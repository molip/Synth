#pragma once

#include "Tag.h"
#include "Graph.h"

#include <memory>

namespace Model
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

	class Graph;
	class GraphCommand : public Command
	{
	public:
		GraphCommand(Graph& graph) : _graph(graph) {}
	protected:
		Graph& _graph;
	};

	class AddModuleCommand : public GraphCommand
	{
	public:
		AddModuleCommand(Tag type, Graph& graph) : GraphCommand(graph), _type(type) {}

		virtual void Do();
		virtual void Undo();
	
	protected:
		Tag _type;
		int _moduleID = 0;
		Graph::AddModuleUndo _undo;
	};

	class RemoveModuleCommand : public GraphCommand
	{
	public:
		RemoveModuleCommand(int moduleID, Graph& graph) : GraphCommand(graph), _moduleID(moduleID) {}

		virtual void Do();
		virtual void Undo();
	
	protected:
		int _moduleID;
		Graph::RemoveModuleUndo _undo;
	};

	class AddConnectionCommand : public GraphCommand
	{
	public:
		AddConnectionCommand(PinRef inputPin, PinRef outputPin, Graph& graph) : GraphCommand(graph), _inputPin(inputPin), _outputPin(outputPin) {}

		virtual void Do();
		virtual void Undo();
	
	protected:
		PinRef _inputPin, _outputPin;
		Graph::ConnectionUndo _undo;
	};

	class RemoveConnectionCommand : public GraphCommand
	{
	public:
		RemoveConnectionCommand(PinRef inputPin, Graph& graph) : GraphCommand(graph), _inputPin(inputPin) {}

		virtual void Do();
		virtual void Undo();
	
	protected:
		PinRef _inputPin;
		Graph::ConnectionUndo _undo;
	};

	class SetPositionCommand : public GraphCommand
	{
	public:
		SetPositionCommand(int modID, const Point& pos, Graph& graph) : GraphCommand(graph), _modID(modID), _pos(pos) {}

		virtual void Do();
		virtual void Undo();
	
	protected:
		int _modID;
		Point _pos, _oldPos;
	};
}
