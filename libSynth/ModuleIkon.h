#pragma once

#include "Enums.h"

#include "Model/Geometry.h"
#include "Model/Tag.h"

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
		ModuleIkon(const Model::Module& module, const Model::Graph& graph);

		int GetModuleID() const;

		Model::Rect GetRect() const;
		const std::string& GetName() const;
		Colour GetColour() const;

		struct Pin
		{
			std::string name;
			Model::Rect rect;
			Model::Point connectionPoint;
			Colour colour;
			Model::Tag id;
		};

		const ModuleIkon::Pin* FindPin(Model::Tag id, bool output);

		const std::vector<Pin>& GetInputPins() const { return GetPins(false); }
		const std::vector<Pin>& GetOutputPins() const { return GetPins(true); }
		const std::vector<Pin>& GetPins(bool outputs) const;

	private:
		void CreatePins(bool outputs) const;
		
		mutable std::vector<Pin> _inputs, _outputs;
		const Model::Module& _module;
		const Model::Graph& _graph;
	};
	}
}