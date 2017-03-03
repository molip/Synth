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

std::vector<ModuleIkon::Pin> ModuleIkon::GetPins(bool outputs) const
{
	std::vector<ModuleIkon::Pin> pins;

	auto& pinDefs = outputs ? _module.GetDef().GetOutputs() : _module.GetDef().GetInputs();
	auto modRect = GetRect();

	if (!pinDefs.empty())
	{
		const int left = outputs ? modRect.Right() - 1 : modRect.Left() + 1 - PinWidth;
		const int top = modRect.Top() + (modRect.Height() - ((int)pinDefs.size() * (PinHeight + PinGap) - PinGap)) / 2;

		Model::Rect rect(left, top, left + PinWidth, 0);
		for (auto& pin : pinDefs)
		{
			rect.Bottom() = rect.Top() + PinHeight;
			pins.push_back(Pin{pin.GetName(), rect});
			rect.Top() += PinHeight + PinGap;
		}
	}

	return pins;
}
