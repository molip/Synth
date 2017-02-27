#include "stdafx.h"
#include "Module.h"
#include "Graph.h"
#include "ModuleTypes.h"

#include "../../libKernel/Serial.h"

using namespace Model;

void PinRef::Save(Serial::SaveNode& node) const
{
	node.SaveType("sourceType", type);
	node.SaveType("sourceModule", moduleID);
}

void PinRef::Load(Serial::LoadNode& node)
{
	node.LoadType("sourceType", type);
	node.LoadType("sourceModule", moduleID);
}


Module::Module(Tag type) : _type(type)
{
}

const ModuleType& Module::GetDef() const
{
	return *ModuleTypes::Find(_type);
}

const PinType& Module::GetInputDef(Tag type) const
{
	return *GetDef().GetInput(type);
}

const PinType& Module::GetOutputDef(Tag type) const
{
	return *GetDef().GetOutput(type);
}

const Module& Module::GetSourceModule(const PinRef& pin, const Graph& graph) const
{
	return *graph.FindModule(pin.moduleID);
}

const PinType& Module::GetSourceOutputDef(const PinRef& pin, const Graph& graph) const
{
	return GetSourceModule(pin, graph).GetOutputDef(pin.type);
}

void Module::SetValue(Tag inputType, int value)
{
	_values[inputType] = value;
}

bool Module::IsInstanced(const Graph& graph) const
{
	for (auto& pair : _connections)
	{
		if (!GetInputDef(pair.first).IsMulti())
		{
			if (GetSourceOutputDef(pair.second, graph).IsMulti())
				return true;

			if (GetSourceModule(pair.second, graph).IsInstanced(graph))
				return true;
		}
	}

	return false;
}

bool Module::IsDependentOn(int modID, const Graph& graph, bool recurse) const
{
	if (modID == _id)
		return false;

	for (auto& pair: _connections)
		if (pair.second.moduleID == modID)
			return true;

	if (recurse)
		for (auto& pair: _connections)
			if (GetSourceModule(pair.second, graph).IsDependentOn(modID, graph, true))
				return true;

	return false;
}

void Module::Load(Serial::LoadNode& node)
{
	node.LoadType("id", _id);
	node.LoadType("type", _type);
	node.LoadMap("connections", _connections, Serial::TypeLoader(), Serial::ClassLoader());
	node.LoadMap("values", _values, Serial::TypeLoader(), Serial::TypeLoader());
}

void Module::Save(Serial::SaveNode& node) const
{
	node.SaveType("id", _id);
	node.SaveType("type", _type);
	node.SaveMap("connections", _connections, Serial::TypeSaver(), Serial::ClassSaver());
	node.SaveMap("values", _values, Serial::TypeSaver(), Serial::TypeSaver());
}

Module::ConnectionUndo Module::AddConnection(Tag inputType, PinRef outputPin)
{
	ConnectionUndo undo;
	auto& source = _connections[inputType];
	undo.inputType = inputType;
	undo.source = source;
	source = outputPin;
	return undo;
}

Module::ConnectionUndo Module::RemoveConnection(Tag inputType)
{
	ConnectionUndo undo;
	undo.source = _connections[inputType];
	undo.inputType = inputType;
	_connections.erase(undo.inputType);
	return undo;
}

std::vector<Module::ConnectionUndo> Module::RemoveConnectionsToSource(int moduleID)
{
	std::vector<Module::ConnectionUndo> undos;

	for (auto& it = _connections.begin(); it != _connections.end(); )
	{
		if (it->second.moduleID == moduleID)
		{
			undos.push_back(ConnectionUndo{it->first, it->second});
			it = _connections.erase(it);
		}
		else
			++it;
	}
	return undos;
}

void Module::ApplyUndo(const ConnectionUndo& undo)
{
	if (undo.source.moduleID)
		_connections[undo.inputType] = undo.source;
	else
		_connections.erase(undo.inputType);
}
