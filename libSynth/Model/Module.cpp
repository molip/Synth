#include "stdafx.h"
#include "Module.h"
#include "Graph.h"
#include "ModuleTypes.h"

#include "../../libKernel/Serial.h"

using namespace Synth::Model;

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


void InputParams::Save(Serial::SaveNode& node) const
{
	node.SaveType("offset", offset);
	node.SaveType("scale", scale);
}

void InputParams::Load(Serial::LoadNode& node)
{
	node.LoadType("offset", offset);
	node.LoadType("scale", scale);
}

bool InputParams::operator ==(const InputParams& rhs) const
{
	return rhs.offset == offset && rhs.scale == scale;
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

const PinRef* Module::FindConnection(Tag type) const
{
	auto it = _connections.find(type);
	return it == _connections.end() ? nullptr : &it->second;
}

void Module::SetInputParams(Tag inputType, InputParams params)
{
	auto* valDef = GetInputDef(inputType).GetValueType();
	KERNEL_VERIFY(valDef);
	
	if (valDef->GetDefault() == params) // TODO: What if the default changes?
		_inputParams.erase(inputType);
	else
		_inputParams[inputType] = params;
}

const InputParams* Module::FindInputParams(Tag type) const
{
	auto it = _inputParams.find(type);
	if (it != _inputParams.end())
		return &it->second;

	return GetDefaultInputParams(type);
}

const InputParams* Module::GetDefaultInputParams(Tag type) const
{
	if (const auto* pinType = GetDef().GetInput(type))
		if (const auto* valDef = pinType->GetValueType())
			return &valDef->GetDefault();

	return nullptr;
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
	node.LoadType("position", _position);
	node.LoadMap("connections", _connections, Serial::TypeLoader(), Serial::ClassLoader());
	
	if (!node.LoadMap("input_params", _inputParams, Serial::TypeLoader(), Serial::ClassLoader()))
	{
		std::map<Tag, int> values;
		node.LoadMap("values", values, Serial::TypeLoader(), Serial::TypeLoader());

		for (auto& input : GetDef().GetInputs())
		{
			if (const InputParams* defaults = GetDefaultInputParams(input->GetID()))
			{
				bool connected = !!FindConnection(input->GetID());
				auto& it = values.find(input->GetID());
				if (connected || it != values.end())
					_inputParams[input->GetID()] = { connected ? 0 : it->second, defaults->scale };
			}
		}
	}
}

void Module::Save(Serial::SaveNode& node) const
{
	node.SaveType("id", _id);
	node.SaveType("type", _type);
	node.SaveType("position", _position);
	node.SaveMap("connections", _connections, Serial::TypeSaver(), Serial::ClassSaver());
	node.SaveMap("input_params", _inputParams, Serial::TypeSaver(), Serial::ClassSaver());
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
