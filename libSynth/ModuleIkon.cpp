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
	const int Width = 252;
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
	const int pinCount = (int)_module.GetDef().GetFields().size() + (int)std::max(_module.GetDef().GetOutputs().size(), _module.GetDef().GetInputs().size());
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

const ModuleIkon::Field* ModuleIkon::FindField(Model::Tag id)
{
	for (auto& field : _fields)
		if (field.id == id)
			return &field;

	return nullptr;
}

const std::vector<ModuleIkon::Pin>& ModuleIkon::GetPins(bool outputs) const
{
	Populate();
	return outputs ? _outputs : _inputs;
}

const std::vector<ModuleIkon::Field>& ModuleIkon::GetFields() const 
{
	Populate();
	return _fields; 
}

void ModuleIkon::Populate() const
{
	if (_fields.empty() && _inputs.empty() && _outputs.empty())
	{
		const int top = CreateFields();
		CreatePins(false, top);
		CreatePins(true, top);
	}
}

int ModuleIkon::CreateFields() const
{
	const Model::Rect& modRect = GetLabelRect();
	Model::Rect rect(modRect.Left(), modRect.Bottom(), modRect.Right(), modRect.Bottom() + PinHeight);
	
	for (const auto& fieldDef : _module.GetDef().GetFields())
	{
		const Model::FieldParams params = _module.FindFieldParams(fieldDef->GetID());

		_fields.push_back({ rect, fieldDef->GetID(), params.content});
		rect.Offset(0, PinHeight + PinGap);
	}

	return rect.Top();
}

void ModuleIkon::CreatePins(bool outputs, int top) const
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
		Model::Rect connectionRect(modRect.Left() + 1 - ConnectionWidth, top, modRect.Left(), top + PinHeight);
		Model::Rect labelRect(modRect.Left(), top, modRect.Left() + LabelWidth, top + PinHeight);
		Model::Rect offsetRect(labelRect.Right(), top, labelRect.Right() + ValueWidth, top + PinHeight);
		Model::Rect scaleRect(offsetRect.Right() + PinGap, top, offsetRect.Right() + PinGap + ValueWidth, top + PinHeight);
		
		if (outputs)
		{
			connectionRect.MirrorX(modRect.GetCentre().x);
			labelRect.MirrorX(modRect.GetCentre().x);
			offsetRect = scaleRect = Model::Rect();
		}
		
		for (auto& pinDef : pinDefs)
		{
			Pin pin{ pinDef->GetName(), labelRect, connectionRect, offsetRect, scaleRect, outputs, pinDef->IsMulti() ? Colour::Red : Colour::Blue, pinDef->GetID() };

			if (pinDef->IsMonitor())
			{
				pin.monitorArea.rect = labelRect;
				pin.monitorArea.rect.Offset(0, labelRect.Height());
				pin.monitorArea.rect.Inflate(0, 0, -4, 0);
				pin.monitorArea.pinRef.moduleID = _module.GetID();
				pin.monitorArea.pinRef.type = pinDef->GetID();
			}

			if (!outputs)
				if (const auto* valType = pinDef->GetValueType())
				{
					Model::InputParams params = *_module.FindInputParams(pin.id);
					pin.offset = valType->ToString(params.offset);
					pin.scale = valType->ToString(params.scale);
					pin.showOffset = true;
					pin.showScale = _module.FindConnection(pin.id);
				}

			pins.push_back(pin);

			const int pitch = PinHeight + PinGap;
			connectionRect.Offset(0, pitch);
			labelRect.Offset(0, pitch);
			offsetRect.Offset(0, pitch);
			scaleRect.Offset(0, pitch);
		}
	}
}
