#pragma once

#include "Enums.h"

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
		ModuleIkon(const Model::Module& module, const Model::Graph& graph) : _module(module), _graph(graph) {}

		int GetModuleID() const;

		Model::Rect GetRect() const;
		const std::string& GetName() const;
		Colour GetColour() const;

		struct Pin
		{
			std::string name;
			Model::Rect rect;
			Colour colour;
		};

		std::vector<Pin> GetInputPins() const { return GetPins(false); }
		std::vector<Pin> GetOutputPins() const { return GetPins(true); }

	private:
		std::vector<ModuleIkon::Pin> GetPins(bool outputs) const;

		const Model::Module& _module;
		const Model::Graph& _graph;
	};
	}
}