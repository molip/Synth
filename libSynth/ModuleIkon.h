#pragma once

#include "Model/Geometry.h"

#include <vector>

namespace Synth
{
	namespace Model
	{
		class Graph;
		class Module;
	}
	class CommandStack;

	namespace UI
	{
	class ModuleIkon
	{
	public:
		ModuleIkon(const Model::Module& module) : _module(module) {}

		int GetModuleID() const;

		Model::Rect GetRect() const;
		const std::string& GetName() const;

		struct Pin
		{
			std::string name;
			Model::Rect rect;
		};

		std::vector<Pin> GetInputPins() const { return GetPins(false); }
		std::vector<Pin> GetOutputPins() const { return GetPins(true); }

	private:
		std::vector<ModuleIkon::Pin> GetPins(bool outputs) const;

		const Model::Module& _module;
	};
	}
}