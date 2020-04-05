#pragma once

#include "Enums.h"

#include "Model/Geometry.h"
#include "Model/Module.h"
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
		ModuleIkon(const Model::Module& module, bool selected, const Model::Graph& graph);

		int GetModuleID() const;

		Model::Rect GetLabelRect() const;
		Model::Rect GetRect() const;
		const std::string& GetName() const;
		Colour GetColour() const;
		bool IsSelected() const { return _selected; }

		struct MonitorArea
		{
			Model::Rect rect;
			Model::PinRef pinRef;
		};

		struct Pin
		{
			Model::Point GetConnectionPoint() const { return Model::Point(isOutput ? connectionRect.Right() : connectionRect.Left(), connectionRect.GetCentre().y); }

			std::string name;
			Model::Rect labelRect, connectionRect, offsetRect, scaleRect;
			bool isOutput = false;
			Colour colour {};
			Model::Tag id;
			
			bool showOffset = false;
			bool showScale = false;
			std::string offset, scale;
			MonitorArea monitorArea;
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
		const bool _selected;
	};

	struct MonitorIkon
	{
		Model::Rect rect;
		std::vector<int> activeWidths;
	};
	}
}