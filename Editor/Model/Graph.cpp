#include "stdafx.h"
#include "Graph.h"
#include "Module.h"
#include "ModuleTypes.h"
#include <set>

#include "../libKernel/Serial.h"

using namespace Model;

Graph::Graph()
{
	int midi = AddModule("midi");
	int env = AddModule("envl");
	int osc = AddModule("oscl");
	int mixer = AddModule("pmix");
	int target = AddModule("trgt");

	FindModule(env)->Connect("gate", midi, "gate");
	FindModule(osc)->Connect("ptch", midi, "ptch");
	FindModule(osc)->Connect("levl", env, "levl");
	FindModule(mixer)->Connect("sgnl", osc, "sgnl");
	FindModule(target)->Connect("sgnl", mixer, "sgnl");

	FindModule(env)->SetValue("atck", 500);
	FindModule(env)->SetValue("decy", 500);
	FindModule(env)->SetValue("sust", 0x8000);
	FindModule(env)->SetValue("rels", 1000);

	Test();
}

void Graph::Test()
{

	bool a = _modules[0].IsInstanced(*this);
	bool b = _modules[1].IsInstanced(*this);
	bool c = _modules[2].IsInstanced(*this);
	bool d = _modules[3].IsInstanced(*this);
	bool e = _modules[4].IsInstanced(*this);

	SortModules();

	for (auto& mod : _modules)
	{
		Kernel::Debug::Trace << mod.GetDef().GetName() << ":" << std::endl;
		for (auto& input : mod.GetDef().GetInputs())
		{
			Kernel::Debug::Trace << "\t" << input.GetName() << ":" << std::endl;
			std::vector<PinRef> outputRefs = GetValidSourcePins({mod.GetID(), input.GetID()});
			for (auto& outputRef : outputRefs)
				Kernel::Debug::Trace << "\t\t" << outputRef.moduleID << "/" << outputRef.type << std::endl;
		}
	}
}

Graph::~Graph() = default;


int Graph::AddModule(Tag type)
{
	_modules.push_back(Module(type));
    _modules.back().SetID(_nextModuleID);
    return _nextModuleID++;
}

Module* Graph::FindModule(int modID)
{
	for (auto& mod : _modules)
		if (mod.GetID() == modID)
			return &mod;
	return nullptr;
}

void Graph::SortModules()
{
	std::set<int> done;
	std::vector<Module> sorted;

	auto WantDefer = [&](const Module& mod)
	{
		for (auto& pair : mod.GetConnections())
			if (done.count(pair.second.moduleID) == 0) // Source not done yet. 
				return true;
		return false;
	};
	
	while (done.size() < _modules.size())
		for (auto& mod : _modules)
			if (done.count(mod.GetID()) == 0 && !WantDefer(mod))
			{
				sorted.push_back(mod);
				done.insert(mod.GetID());
			}
				
	_modules = sorted;
}
	
// Assumes sorted.
std::vector<PinRef> Graph::GetValidSourcePins(PinRef input)
{
	const PinType& inputDef = FindModule(input.moduleID)->GetInputDef(input.type);

	std::vector<PinRef> result;
		
	// Find earliest non-dependent module.
	for (auto& mod : _modules)
	{
		if (mod.GetID() != input.moduleID)
		{
			if (mod.IsDependentOn(input.moduleID, *this, false))
				break;

			for (auto& outputDef : mod.GetDef().GetOutputs())
				if (outputDef.GetDataType() == inputDef.GetDataType())
					result.push_back(PinRef {mod.GetID(), outputDef.GetID()});
		}
	}

	return result;		
}

void Graph::Save(Serial::SaveNode& node) const
{
	node.SaveCntr("modules", _modules, Serial::ClassSaver());
}

void Graph::Load(Serial::LoadNode& node)
{
	node.LoadCntr("modules", _modules, Serial::ClassLoader());
}
