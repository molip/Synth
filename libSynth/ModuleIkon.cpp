#include "stdafx.h"
#include "ModuleIkon.h"
#include "Model/Module.h"
#include "Model/ModuleTypes.h"

#include <algorithm>

using namespace Synth;
using namespace Synth::UI;

namespace
{
	const int PinWidth = 50;
	const int PinHeight = 20;
	const int PinGap = 10;
	const int MinHeight = 50;
	const int Width = 150;
}

ModuleIkon::ModuleIkon(const Model::Module& module, bool selected, const Model::Graph& graph) : _module(module), _selected(selected), _graph(graph) 
{
}

int ModuleIkon::GetModuleID() const 
{
	return _module.GetID();
}

Model::Rect ModuleIkon::GetRect() const
{
	const Model::Point pos = _module.GetPosition();
	const int pinCount = (int)std::max(_module.GetDef().GetOutputs().size(), _module.GetDef().GetInputs().size());
	const int height = std::max(MinHeight, pinCount * (PinHeight + PinGap));

	return Model::Rect(pos, pos + Model::Point(Width, height));
}

const std::string& ModuleIkon::GetName() const
{
	return _module.GetDef().GetName();
}

Colour ModuleIkon::GetColour() const
{
	return _module.IsInstanced(_graph) ? Colour::Red : Colour::Blue;
}

const ModuleIkon::Pin* ModuleIkon::FindPin(Model::Tag id, bool output)
{
	for (auto& pin : GetPins(output))
		if (pin.id == id)
			return &pin;

	return nullptr;
}

const std::vector<ModuleIkon::Pin>& ModuleIkon::GetPins(bool outputs) const
{
	CreatePins(outputs);
	return outputs ? _outputs : _inputs;
}

void ModuleIkon::CreatePins(bool outputs) const
{
	const auto& pinDefs = outputs ? _module.GetDef().GetOutputs() : _module.GetDef().GetInputs();
	if (pinDefs.empty())
		return;
	
	auto& pins = outputs ? _outputs : _inputs;
	if (!pins.empty())
		return;

	auto modRect = GetRect();

	if (!pinDefs.empty())
	{
		const int left = outputs ? modRect.Right() - 1 : modRect.Left() + 1 - PinWidth;
		const int top = modRect.Top() + (modRect.Height() - ((int)pinDefs.size() * (PinHeight + PinGap) - PinGap)) / 2;

		Model::Rect rect(left, top, left + PinWidth, 0);
		for (auto& pin : pinDefs)
		{
			rect.Bottom() = rect.Top() + PinHeight;
			Model::Point connectionPoint(outputs ? rect.Right() : rect.Left(), rect.GetCentre().y);
			pins.push_back(Pin{pin.GetName(), rect, connectionPoint, pin.IsMulti() ? Colour::Red : Colour::Blue, pin.GetID() });
			rect.Top() += PinHeight + PinGap;
		}
	}
}
