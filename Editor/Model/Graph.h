#pragma once

#include "Module.h"
#include "Tag.h"
#include "Defs.h"

#include <memory>
#include <vector>

namespace Model
{
	class Graph
	{
	public:
		Graph();
		~Graph();

		void Test();

        int AddModule(Tag type);
		Module* FindModule(int modID);
		const Module* FindModule(int modID) const { return const_cast<Graph*>(this)->FindModule(modID); }

		void SortModules();
		void Save(Serial::SaveNode& node) const;
		void Load(Serial::LoadNode& node);

		std::vector<PinRef> GetValidSourcePins(PinRef input);
	
	private:
		std::vector<Module> _modules;
		int _nextModuleID = 1;
	};
}