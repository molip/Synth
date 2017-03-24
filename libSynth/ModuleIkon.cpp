#include "stdafx.h"
#include "ModuleIkon.h"
#include "Model/Module.h"
#include "Model/ModuleTypes.h"

#include <algorithm>

using namespace Synth;
using namespace Synth::UI;

namespace
{
	const int PinHeight = 20;
	const int PinGap = 2;
	const int ConnectionWidth = 20;
	const int LabelWidth = 50;
	const int ValueWidth = 50;
	const int MinHeight = 50;
	const int Width = 200;
	const int LabelHeight = 30;
}

ModuleIkon::ModuleIkon(const Model::Module& module, bool selected, const Model::Graph& graph) : _module(module), _selected(selected), _graph(graph) 
{
}

int ModuleIkon::GetModuleID() const 
{
	return _module.GetID();
}

Model::Rect ModuleIkon::GetLabelRect() const
{
	const Model::Point pos = _module.GetPosition();
	return Model::Rect(pos, pos + Model::Point(Width, LabelHeight));
}

Model::Rect ModuleIkon::GetRect() const
{
	Model::Rect rect = GetLabelRect();
	const int pinCount = (int)std::max(_module.GetDef().GetOutputs().size(), _module.GetDef().GetInputs().size());
	rect.Bottom() += pinCount * (PinHeight + PinGap) + PinGap;
	return rect;
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

	auto modRect = GetLabelRect();

	if (!pinDefs.empty())
	{
		const int top = modRect.Bottom();

		Model::Rect connectionRect(modRect.Left() + 1 - ConnectionWidth, top, modRect.Left(), top + PinHeight);
		Model::Rect labelRect(modRect.Left(), top, modRect.Left() + LabelWidth, top + PinHeight);
		Model::Rect valueRect(labelRect.Right(), top, labelRect.Right() + ValueWidth, top + PinHeight);
		
		if (outputs)
		{
			connectionRect.MirrorX(modRect.GetCentre().x);
			labelRect.MirrorX(modRect.GetCentre().x);
			valueRect = Model::Rect();
		}
		
		for (auto& pinDef : pinDefs)
		{
			Pin pin{ pinDef->GetName(), labelRect, connectionRect, valueRect, outputs, pinDef->IsMulti() ? Colour::Red : Colour::Blue, pinDef->GetID(), pinDef->IsInternal() };
			
			if (!outputs && !_module.FindConnection(pin.id))
				if (const auto* valType = pinDef->GetValueType())
				{
					pin.value = valType->ToString(*_module.FindValue(pin.id));
					pin.showValue = true;
				}

			pins.push_back(pin);
			connectionRect.Offset(0, PinHeight + PinGap);
			labelRect.Offset(0, PinHeight + PinGap);
			valueRect.Offset(0, PinHeight + PinGap);
		}
	}
}
