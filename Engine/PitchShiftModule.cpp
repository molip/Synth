#include "PitchShiftModule.h"
#include "Util.h"

using namespace Engine;

PitchShiftModule::PitchShiftModule()
{
	_inputs.SetSize(Pin::PitchShift::Input::_Count);
	_outputs.SetSize(Pin::PitchShift::Output::_Count);
}

void PitchShiftModule::Update()
{
	Input& shiftInput = _inputs[Pin::PitchShift::Input::Shift];
	Input& pitchInput = _inputs[Pin::PitchShift::Input::Pitch];
	
	if (shiftInput.HasChanged() || pitchInput.HasChanged())
	{
		shiftInput.ResetChanged();
		pitchInput.ResetChanged();

		const int16_t shift = static_cast<int16_t>(shiftInput.GetValue());
		float pitch = pitchInput.GetValue() + shift * Config::pitchPerSemitone;
		
		_outputs[Pin::PitchShift::Output::Pitch].SetValue(pitch);
	}
}
