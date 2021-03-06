#include "stdafx.h"
#include "Graph.h"
#include "Module.h"
#include "ModuleTypes.h"
#include "Notification.h"
#include <algorithm>
#include <set>

#include "../../libKernel/Serial.h"

using namespace Synth::Model;

Graph::Graph()
{
}

Graph::~Graph() = default;

Module* Graph::FindModule(int modID)
{
	for (auto& mod : _modules)
		if (mod.GetID() == modID)
			return &mod;
	return nullptr;
}

void Graph::SortModules(bool notify)
{
	std::set<int> done;
	_sorted.clear();

	auto AllSourcesDone = [&](const Module& mod)
	{
		for (auto& pair : mod.GetConnections())
			if (done.count(pair.second.moduleID) == 0)
				return false;
		return true;
	};

	auto AnySourcesDone = [&](const Module& mod)
	{
		for (auto& pair : mod.GetConnections())
			if (done.count(pair.second.moduleID) > 0)
				return true;
		return false;
	};

	while (done.size() < _modules.size())
	{
		bool found = false;
		for (auto& mod : _modules)
		{
			if (done.count(mod.GetID()) == 0 && AllSourcesDone(mod))
			{
				_sorted.push_back(&mod);
				done.insert(mod.GetID());
				found = true;
			}
		}

		if (!found) 
		{
			// Must be a feedback loop - try to break the deadlock. 

			auto tryDeadlocked = [&](bool force)
			{
				for (auto& mod : _modules)
				{
					if (done.count(mod.GetID()) == 0 && mod.IsDependentOn(mod.GetID(), *this) && (force || AnySourcesDone(mod)))
					{
						_sorted.push_back(&mod);
						done.insert(mod.GetID());
						return true;
					}
				}
				return false;
			};

			if (!tryDeadlocked(false))
				if (!tryDeadlocked(true))
					KERNEL_VERIFY(false);
		}
	}

	if (notify)
		SendNotification(StructureChangedNotification());
}
	
// Assumes sorted.
std::vector<PinRef> Graph::GetValidSourcePins(PinRef input)
{
	const PinType& inputDef = FindModule(input.moduleID)->GetInputDef(input.type);

	std::vector<PinRef> result;
		
	for (auto& mod : _sorted)
		for (auto& outputDef : mod->GetDef().GetOutputs()) // So add its outputs. 
			result.push_back(PinRef(mod->GetID(), outputDef->GetID()));

	return result;		
}

void Graph::Save(Serial::SaveNode& node) const
{
	node.SaveCntr("modules", _modules, Serial::ClassSaver());
}

void Graph::Load(Serial::LoadNode& node)
{
	node.LoadCntr("modules", _modules, Serial::ClassLoader());

	_nextModuleID = 0;
	for (auto& mod : _modules)
	{
		_nextModuleID = std::max(_nextModuleID, mod.GetID());
	}

	++_nextModuleID;
	SortModules(false);
}

Graph::AddModuleUndo Graph::AddModule(Tag type)
{
	_modules.push_back(Module(type));
    _modules.back().SetID(_nextModuleID);
	SortModules();
	AddModuleUndo undo;
	undo.moduleID = _nextModuleID++;
	return undo;
}

void Graph::ApplyUndo(const AddModuleUndo& undo)
{
	_modules.pop_back();
	--_nextModuleID;
	SortModules();
}

Graph::RemoveModuleUndo Graph::RemoveModule(int moduleID)
{
	RemoveModuleUndo undo;

	for (auto& mod : _modules)
	{
		std::vector<Module::ConnectionUndo> undos = mod.RemoveConnectionsToSource(moduleID);
		if (!undos.empty())
			undo.connections[mod.GetID()] = undos;
	}

	auto it = std::find_if(_modules.begin(), _modules.end(), [&](auto& mod) { return mod.GetID() == moduleID; });
	undo.module = *it;
	undo.position = int(it - _modules.begin());
	_modules.erase(it);
	SortModules();
	return undo;
}

void Graph::ApplyUndo(const RemoveModuleUndo& undo)
{
	_modules.insert(_modules.begin() + undo.position, undo.module);
	for (auto& pair : undo.connections)
	{
		Module* mod = FindModule(pair.first);
		for (auto& connUndo : pair.second)
			mod->ApplyUndo(connUndo);
	}
	SortModules();
}

Graph::ConnectionUndo Graph::AddConnection(PinRef inputPin, PinRef outputPin)
{
	Module* mod = FindModule(inputPin.moduleID);
	ConnectionUndo undo;
	undo.undo = mod->AddConnection(inputPin.type, outputPin);
	undo.moduleID = inputPin.moduleID;
	SortModules();
	return undo;
}

Graph::ConnectionUndo Graph::RemoveConnection(PinRef inputPin)
{
	Module* mod = FindModule(inputPin.moduleID);
	ConnectionUndo undo;
	undo.undo = mod->RemoveConnection(inputPin.type);
	undo.moduleID = inputPin.moduleID;
	SortModules();
	return undo;
}

void Graph::ApplyUndo(const ConnectionUndo& undo)
{
	Module* mod = FindModule(undo.moduleID);
	mod->ApplyUndo(undo.undo);
	SortModules();
}

void Graph::SendNotification(const Notification& notification) const
{
	for (auto& observer : _observers)
		observer->OnGraphNotification(notification);
}
