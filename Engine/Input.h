#pragma once 

#include "Module.h"
#include "Graph.h"

#include "Platform.h"

namespace Engine
{
class Graph;
class Module;

namespace RemoteInput
{
	void Process(byte data);
	extern char* _error;

	template <typename T>
	class Value
	{
	public:
		void AddData(byte data) { _value = _value << 8 | data; ++_read; }
		bool IsFinished() const { return _read >= sizeof(T); }
		operator T() const { return _value; }
	private:
		size_t _size = 0, _read = 0;
		T _value = 0;
	};

	class Command
	{
	public:
		Command(Graph* graph) : _graph(graph) {}
		virtual ~Command() = default;
		virtual Error AddData(byte data) = 0; // Returns error code. 
		virtual bool Execute() const = 0; // Returns true if finished. 
	protected:
		Graph* _graph;
	};

	class InitGraphCommand : public Command
	{
	public:
		InitGraphCommand(Graph* graph) : Command(graph) {}
		virtual Error AddData(byte data) override;
		virtual bool Execute() const override;

	private:
		int _modCount = -1;
		int _polyModCount = -1;
		int _polyphony = -1;
	};

	class AddModuleCommand : public Command
	{
	public:
		AddModuleCommand(Graph* graph, bool poly) : Command(graph), _poly(poly) {}
		virtual Error AddData(byte data) override;
		virtual bool Execute() const override;

	protected:
		ModuleType _moduleType = ModuleType::None;
		bool _poly;
	};

	class AddMonoModuleCommand : public AddModuleCommand 
	{
	public:
		AddMonoModuleCommand(Graph* graph) : AddModuleCommand(graph, false) {}
	};

	class AddPolyModuleCommand : public AddModuleCommand 
	{
	public:
		AddPolyModuleCommand(Graph* graph) : AddModuleCommand(graph, true) {}
	};

	class Pin
	{
	public:
		void AddData(byte data);
		bool IsMono() const { return _modType == InstanceType::Mono; }

		InstanceType _modType = InstanceType::None;
		int _modIndex = -1;
		int _pinIndex = -1;
		bool _done = false;
	};
	
	class AddConnectionCommand : public Command
	{
	public:
		AddConnectionCommand(Graph* graph) : Command(graph) {}
		virtual Error AddData(byte data) override;
		virtual bool Execute() const override;

		class Connection
		{
		public:
			void AddData(byte data) { _pin.AddData(data); }
			bool IsDone() const { return _pin._done; }

			template <typename T> T& GetMonoSinglePin(Graph& graph) const
			{
				return graph.GetMonoModule(_pin._modIndex)->GetPins<T>()[_pin._pinIndex];
			}

			template <typename T> T& GetMonoMultiPin(Graph& graph, int i) const
			{
				return graph.GetMonoModule(_pin._modIndex)->GetMultiPins<T>()[_pin._pinIndex][i];
			}

			template <typename T> T& GetPolySinglePin(Graph& graph, int i) const 
			{
				return graph.GetPolyModule(_pin._modIndex, i)->GetPins<T>()[_pin._pinIndex];
			}

			template <typename T> T& GetPin(Graph& graph, int i, bool multi) const 
			{
				return _pin.IsMono() ? multi ? GetMonoMultiPin<T>(graph, i) : GetMonoSinglePin<T>(graph) : GetPolySinglePin<T>(graph, i);
			}

			void ConnectToOutput(Graph& graph, const Connection& output, bool multi) const
			{
				if (_pin.IsMono() && output._pin.IsMono() && !multi)
					GetMonoSinglePin<Input>(graph).Connect(output.GetMonoSinglePin<Output>(graph));
				else
					for (int i = 0; i < graph.GetPolyphony(); ++i)
						GetPin<Input>(graph, i, multi).Connect(output.GetPin<Output>(graph, i, multi));
			}

			Pin _pin;
		};

	private:
		ConnectionType _connType = ConnectionType::None;
		Connection _input, _output;
	};

	class AddMonitorCommand : public Command
	{
	public:
		AddMonitorCommand(Graph* graph) : Command(graph) {}
		
		virtual Error AddData(byte data) override
		{
			if (_pin._done)
				return Error::TooManyParameters;

			_pin.AddData(data);
			return Error::None;
		}

		virtual bool Execute() const override
		{
			if (!_pin._done)
				return false;

			_graph->AddMonitor(_pin.IsMono(), _pin._modIndex, _pin._pinIndex);
			return true;
		}

	protected:
		Pin _pin;
	};

	class SetInputParamsCommand : public Command
	{
	public:
		SetInputParamsCommand(Graph* graph) : Command(graph) {}
		virtual Error AddData(byte data) override
		{
			if (_poly < 0)
				_poly = data;
			else if (_modIndex < 0)
				_modIndex = data;
			else if (_pinIndex < 0)
				_pinIndex = data;
			else if (!_offset.IsFinished())
				_offset.AddData(data);
			else if (!_scale.IsFinished())
				_scale.AddData(data);
			else
				return Error::TooManyParameters;

			return Error::None;
		}

		virtual bool Execute() const override
		{
			if (!_scale.IsFinished())
				return false;

			static_assert(sizeof(float) == 4, "Wrong float size");
			
			auto bytesToFloat = [](uint32_t bytes)
			{
				union { float f; uint32_t i; } u;
				u.i = bytes;
				return u.f;
			};

			float offset = bytesToFloat(_offset);
			float scale = bytesToFloat(_scale);

			if (_poly)
			{
				for (int i = 0; i < _graph->GetPolyphony(); ++i)
					_graph->GetPolyModule(_modIndex, i)->GetPins<Input>()[_pinIndex].SetParams(offset, scale);
			}
			else
				_graph->GetMonoModule(_modIndex)->GetPins<Input>()[_pinIndex].SetParams(offset, scale);

			return true;
		}

	protected:
		int _poly = -1;
		int _modIndex = -1;
		int _pinIndex = -1;
		Value<uint32_t> _offset, _scale;
	};

	class SetFieldParamsCommand : public Command
	{
	public:
		SetFieldParamsCommand(Graph* graph) : Command(graph) {}
		virtual Error AddData(byte data) override
		{
			if (_modIndex < 0)
				_modIndex = data;
			else if (_fieldIndex < 0)
				_fieldIndex = data;
			else if (_length < 0)
			{
				_length = data;
				_values = new Field::DataType;
				_values->Reserve(_length);
			}
			else if (_values->GetSize() < _length)
				_values->Push(data);
			else
				return Error::TooManyParameters;

			return Error::None;
		}

		virtual bool Execute() const override
		{
			if (!_values || _values->GetSize() < _length)
				return false;

			_graph->GetMonoModule(_modIndex)->GetFields()[_fieldIndex].AttachData(_values);
			return true;
		}

	protected:
		int _modIndex = -1;
		int _fieldIndex = -1;
		int _length = -1;
		Field::DataType* _values = nullptr;
	};

	class SetMIDIDataCommand : public Command
	{
	public:
		SetMIDIDataCommand(Graph* graph) : Command(graph) {}
		virtual Error AddData(byte data) override;
		virtual bool Execute() const override;

	protected:
		Value<int16_t> _division;
		Value<uint32_t> _dataSize;
		byte* _data = nullptr;
		uint32_t _dataRead = 0;
		bool _gotError = false;
	};

	class MIDIInputCommand : public Command
	{
	public:
		MIDIInputCommand(Graph* graph) : Command(graph) {}
		virtual Error AddData(byte data) override;
		virtual bool Execute() const override;

	protected:
		Value<uint32_t> _dataSize;
		byte* _data = nullptr;
		uint32_t _dataRead = 0;
		bool _gotError = false;
	};

	class SettingsCommand : public Command
	{
	public:
		SettingsCommand(Graph* graph) : Command(graph) {}
		virtual Error AddData(byte data) override;
		virtual bool Execute() const override;

	protected:
		Value<bool> _hold;
		Value<bool> _arpEnabled;
		Value<uint16_t> _arpPeriod;
		Value<uint16_t> _arpDuty;
		Value<uint16_t> _arpOctaves;
	};

}
}