#pragma once

#include "Model/Tag.h"
#include "Model/Graph.h"

#include <memory>

namespace Synth
{
	class Command
	{
	public:
		virtual ~Command() = default;

		virtual void Do() {}
		virtual void Undo() {}
		virtual void Redo() { Do(); }
	};
	using CommandPtr = std::unique_ptr<Command>;

	namespace Model
	{
		class Graph;
	}
	class GraphCommand : public Command
	{
	public:
		GraphCommand(Model::Graph& graph) : _graph(graph) {}
	protected:
		Model::Graph& _graph;
	};

	class AddModuleCommand : public GraphCommand
	{
	public:
		AddModuleCommand(Model::Tag type, Model::Graph& graph) : GraphCommand(graph), _type(type) {}

		virtual void Do();
		virtual void Undo();
	
	protected:
		Model::Tag _type;
		int _moduleID = 0;
		Model::Graph::AddModuleUndo _undo;
	};

	class RemoveModuleCommand : public GraphCommand
	{
	public:
		RemoveModuleCommand(int moduleID, Model::Graph& graph) : GraphCommand(graph), _moduleID(moduleID) {}

		virtual void Do();
		virtual void Undo();
	
	protected:
		int _moduleID;
		Model::Graph::RemoveModuleUndo _undo;
	};

	class AddConnectionCommand : public GraphCommand
	{
	public:
		AddConnectionCommand(Model::PinRef inputPin, Model::PinRef outputPin, Model::Graph& graph) : GraphCommand(graph), _inputPin(inputPin), _outputPin(outputPin) {}

		virtual void Do();
		virtual void Undo();
	
	protected:
		Model::PinRef _inputPin, _outputPin;
		Model::Graph::ConnectionUndo _undo;
	};

	class RemoveConnectionCommand : public GraphCommand
	{
	public:
		RemoveConnectionCommand(Model::PinRef inputPin, Model::Graph& graph) : GraphCommand(graph), _inputPin(inputPin) {}

		virtual void Do();
		virtual void Undo();
	
	protected:
		Model::PinRef _inputPin;
		Model::Graph::ConnectionUndo _undo;
	};

	class SetPositionCommand : public GraphCommand
	{
	public:
		SetPositionCommand(int modID, const Model::Point& pos, bool relative, Model::Graph& graph) : GraphCommand(graph), _modID(modID), _pos(pos), _relative(relative) {}

		virtual void Do();
		virtual void Undo();
	
	protected:
		int _modID;
		Model::Point _pos, _oldPos;
		bool _relative;
	};
	
	class SetValueCommand : public GraphCommand
	{
	public:
		SetValueCommand(int modID, Model::Tag pinID, int value, Model::Graph& graph) : GraphCommand(graph), _modID(modID), _pinID(pinID), _value(value) {}

		virtual void Do();
		virtual void Undo();
	
	protected:
		int _modID;
		Model::Tag _pinID;
		int _value;
		int _oldValue = 0;
	};
}
