#pragma once 

#include "Module.h"
#include "Graph.h"

#include <arduino.h>

class Graph;
class Module;

namespace Input
{
	void Process(byte data);

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

			template <typename T> T& GetPolyPin(Graph& graph, int i) const 
			{
				if (_modType == InstanceType::Mono) // Poly pin. 
					return graph.GetMonoModule(_modIndex)->GetPolyPins<T>()[_pinIndex][i];
				else // Poly module. 
					return graph.GetPolyModule(_modIndex, i)->GetPins<T>()[_pinIndex];
			}

			template <typename TVal> void ConnectToOutputMono(Graph& graph, const Connection& output) const 
			{
				GetMonoPin<InputT<TVal>>(graph).Connect(output.GetMonoPin<OutputT<TVal>>(graph));
			}

			template <typename TVal> void ConnectToOutputPoly(Graph& graph, const Connection& output) const 
			{
				for (int i = 0; i < graph.GetPolyphony(); ++i)
					GetPolyPin<InputT<TVal>>(graph, i).Connect(output.GetPolyPin<OutputT<TVal>>(graph, i));
			}

			InstanceType _modType = InstanceType::None;
			PinType _pinType = PinType::None;
			int _modIndex = -1;
			int _pinIndex = -1;
			bool _done = false;
		};

	private:
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
		Module::unsigned_t _val = 0;
		int _valBytes = 0;
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
}
