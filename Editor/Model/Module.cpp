#include "stdafx.h"
#include "Module.h"
#include "Graph.h"
#include "ModuleTypes.h"

#include "../libKernel/Serial.h"

using namespace Model;

void Connection::Save(Serial::SaveNode& node) const
{
	node.SaveType("type", type);
	node.SaveType("sourceType", source.type);
	node.SaveType("sourceModule", source.moduleID);
}

void Connection::Load(Serial::LoadNode& node)
{
	node.LoadType("type", type);
	node.LoadType("sourceType", source.type);
	node.LoadType("sourceModule", source.moduleID);
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

const Module& Module::GetSourceModule(const Connection& conn, const Graph& graph) const
{
	return *graph.FindModule(conn.source.moduleID);
}

const PinType& Module::GetSourceOutputDef(const Connection& conn, const Graph& graph) const
{
	return GetSourceModule(conn, graph).GetOutputDef(conn.source.type);
}

void Module::Connect(Tag inputType, int modID, Tag outputType)
{
	// TODO: Replace.
	_connections.push_back(Connection{inputType, PinRef{modID, outputType}});
}


bool Module::IsInstanced(const Graph& graph) const
{
	for (auto& conn : _connections)
	{
		if (!GetInputDef(conn.type).IsPoly())
		{
			if (GetSourceOutputDef(conn, graph).IsPoly())
				return true;

			if (GetSourceModule(conn, graph).IsInstanced(graph))
				return true;
		}
	}

	return false;
}

bool Module::IsDependentOn(int modID, const Graph& graph, bool recurse) const
{
	if (modID == _id)
		return false;

	for (auto& conn: _connections)
		if (conn.source.moduleID == modID)
			return true;

	if (recurse)
		for (auto& conn: _connections)
			if (GetSourceModule(conn, graph).IsDependentOn(modID, graph, true))
				return true;

	return false;
}

void Module::Load(Serial::LoadNode& node)
{
	node.LoadType("id", _id);
	node.LoadType("type", _type);
	node.LoadCntr("connections", _connections, Serial::ClassLoader());
}

void Module::Save(Serial::SaveNode& node) const
{
	node.SaveType("id", _id);
	node.SaveType("type", _type);
	node.SaveCntr("connections", _connections, Serial::ClassSaver());
}
