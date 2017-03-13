#pragma once 

#include "Module.h"
#include "Graph.h"

#include <arduino.h>

class Graph;
class Module;

namespace Input
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
		int _size = 0, _read = 0;
		T _value = 0;
	};

	class Command
	{
	public:
		virtual ~Command() = default;
		virtual Error AddData(byte data) = 0; // Returns error code. 
		virtual bool Execute(Graph& graph) const = 0; // Returns true if finished. 
	};

	class InitGraphCommand : public Command
	{
	public:
		virtual Error AddData(byte data) override;
		virtual bool Execute(Graph& graph) const override;

	private:
		int _modCount = -1;
		int _polyModCount = -1;
		int _polyphony = -1;
	};

	class AddModuleCommand : public Command
	{
	public:
		AddModuleCommand(bool poly) : _poly(poly) {}
		virtual Error AddData(byte data) override;
		virtual bool Execute(Graph& graph) const override;

	protected:
		ModuleType _moduleType = ModuleType::None;
		bool _poly;
	};

	class AddMonoModuleCommand : public AddModuleCommand 
	{
	public:
		AddMonoModuleCommand() : AddModuleCommand(false) {}
	};

	class AddPolyModuleCommand : public AddModuleCommand 
	{
	public:
		AddPolyModuleCommand() : AddModuleCommand(true) {}
	};

	class AddConnectionCommand : public Command
	{
	public:
		virtual Error AddData(byte data) override;
		virtual bool Execute(Graph& graph) const override;

		class Connection
		{
		public:
			void AddData(byte data); // Returns true if data accepted. 
			bool IsMono() const { return _modType == InstanceType::Mono; }
			bool IsSigned() const { return _pinType == PinType::Signed; }

			template <typename T> T& GetMonoPin(Graph& graph) const 
			{
				return graph.GetMonoModule(_modIndex)->GetPins<T>()[_pinIndex];
			}

			template <typename T> T& GetMultiPin(Graph& graph, int i) const 
			{
				return graph.GetMonoModule(_modIndex)->GetMultiPins<T>()[_pinIndex][i];
			}

			template <typename T> T& GetPolyPin(Graph& graph, int i) const 
			{
				return graph.GetPolyModule(_modIndex, i)->GetPins<T>()[_pinIndex];
			}

			template <typename T> T& GetPin(Graph& graph, int i, bool multi) const 
			{
				return IsMono() ? multi ? GetMultiPin<T>(graph, i) : GetMonoPin<T>(graph) : GetPolyPin<T>(graph, i);
			}

			template <typename TVal> void ConnectToOutput(Graph& graph, const Connection& output, bool multi) const 
			{
				if (IsMono() && output.IsMono())
					GetMonoPin<InputT<TVal>>(graph).Connect(output.GetMonoPin<OutputT<TVal>>(graph));
				else 
					for (int i = 0; i < graph.GetPolyphony(); ++i)
						GetPin<InputT<TVal>>(graph, i, multi).Connect(output.GetPin<OutputT<TVal>>(graph, i, multi));
			}

			InstanceType _modType = InstanceType::None;
			PinType _pinType = PinType::None;
			int _modIndex = -1;
			int _pinIndex = -1;
			bool _done = false;
		};

	private:
		ConnectionType _connType = ConnectionType::None;
		Connection _input, _output;
	};

	class SetUnsignedValueCommand : public Command
	{
	public:
		SetUnsignedValueCommand(bool poly) : _poly(poly) {}
		virtual Error AddData(byte data) override;
		virtual bool Execute(Graph& graph) const override;

	protected:
		bool _poly;
		int _modIndex = -1;
		Value<Module::unsigned_t> _val;
	};

	class SetMonoUnsignedValueCommand : public SetUnsignedValueCommand 
	{
	public:
		SetMonoUnsignedValueCommand() : SetUnsignedValueCommand(false) {}
	};

	class SetPolyUnsignedValueCommand : public SetUnsignedValueCommand 
	{
	public:
		SetPolyUnsignedValueCommand() : SetUnsignedValueCommand(true) {}
	};

	class SetMIDIDataCommand : public Command
	{
	public:
		SetMIDIDataCommand() {}
		virtual Error AddData(byte data) override;
		virtual bool Execute(Graph& graph) const override;

	protected:
		Value<int16_t> _division;
		Value<uint32_t> _dataSize;
		byte* _data = nullptr;
		uint32_t _dataRead = 0;
		bool _gotError = false;
	};

}
