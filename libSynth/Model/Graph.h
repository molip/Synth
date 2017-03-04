#pragma once

#include "Module.h"
#include "Tag.h"
#include "Defs.h"

#include <memory>
#include <vector>

namespace Synth
{
namespace Model
{
	class Graph
	{
	public:
		Graph();
		~Graph();

		Module* FindModule(int modID);
		const Module* FindModule(int modID) const { return const_cast<Graph*>(this)->FindModule(modID); }
		std::vector<Module*>& GetSorted() { return _sorted; } 
		const std::vector<const Module*>& GetSorted() const { return reinterpret_cast<const std::vector<const Module*>&>(_sorted); } 
		void SortModules();
		void Save(Serial::SaveNode& node) const;
		void Load(Serial::LoadNode& node);

		std::vector<PinRef> GetValidSourcePins(PinRef input);
	
		struct AddModuleUndo
		{
			int moduleID = 0;
		};
		AddModuleUndo AddModule(Tag type);
		void ApplyUndo(const AddModuleUndo& undo);

		struct RemoveModuleUndo
		{
			Module module;
			std::map<int, std::vector<Module::ConnectionUndo>> connections; // Module ID.
			int position = 0;
		};
		RemoveModuleUndo RemoveModule(int moduleID);
		void ApplyUndo(const RemoveModuleUndo& undo);

		struct ConnectionUndo
		{
			int moduleID = 0;
			Module::ConnectionUndo undo;

		};
		ConnectionUndo AddConnection(PinRef inputPin, PinRef outputPin);
		ConnectionUndo RemoveConnection(PinRef inputPin);
		void ApplyUndo(const ConnectionUndo& undo);

	private:
		std::vector<Module> _modules;
		std::vector<Module*> _sorted;
		int _nextModuleID = 1;
	};
}
}
