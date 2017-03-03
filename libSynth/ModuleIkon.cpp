#include "stdafx.h"
#include "ModuleIkon.h"
#include "Model/Module.h"
#include "Model/ModuleTypes.h"

using namespace Synth;
using namespace Synth::UI;

int ModuleIkon::GetModuleID() const 
{
	return _module.GetID();
}

Model::Rect ModuleIkon::GetRect() const
{
	Model::Point pos = _module.GetPosition();
	return Model::Rect(pos, pos + Model::Point(300, 200));
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
		double unit = 200 / double(pinDefs.size());
		double top = modRect.Top();
		int left = outputs ? modRect.Right() : modRect.Left() - 50;
		Model::Rect rect(left, 0, left + 50, 0);
		for (auto& pin : pinDefs)
		{
			rect.Top() = int(top);
			rect.Bottom() = int(rect.Top() + unit);
			pins.push_back(Pin{pin.GetName(), rect});
			top += unit;
		}
	}

	return pins;
}
