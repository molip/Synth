#include "KnobModule.h"
#include "Config.h"

using namespace Engine;

KnobModule::KnobModule()
{
	_inputs.SetSize(Pin::Knob::Input::_Count);
	_outputs.SetSize(Pin::Knob::Output::_Count);
}

void KnobModule::Initialise()
{
	UpdateValue();
}

void KnobModule::UpdateValue()
{
	_outputs[Pin::Knob::Output::Value].SetValue(_value * Config::analogInToFloat);
}

void KnobModule::UpdateRemote()
{
	auto& remote = _inputs[Pin::Knob::Input::Remote];

	if (remote.HasChanged())
		_outputs[Pin::Knob::Output::Value].SetValue(remote.GetValue());
}

void KnobModule::SetValue(uint16_t val)
{
	if (_value != val)
	{
		_value = val;
		UpdateValue();
	}
}

int KnobModule::GetIndex() const
{
	return static_cast<int>(_inputs[Pin::Knob::Input::Index].GetValue());
}