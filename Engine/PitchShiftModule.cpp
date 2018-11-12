#include "PitchShiftModule.h"
#include "Util.h"

using namespace Engine;

PitchShiftModule::PitchShiftModule()
{
	_signedInputs.SetSize(Pin::PitchShift::SignedInput::_Count);
	_unsignedInputs.SetSize(Pin::PitchShift::UnsignedInput::_Count);
	_unsignedOutputs.SetSize(Pin::PitchShift::UnsignedOutput::_Count);
}

void PitchShiftModule::Update()
{
	SignedInput& shiftInput = _signedInputs[Pin::PitchShift::SignedInput::Shift];
	UnsignedInput& pitchInput = _unsignedInputs[Pin::PitchShift::UnsignedInput::Pitch];
	
	if (shiftInput.HasChanged() || pitchInput.HasChanged())
	{
		shiftInput.ResetChanged();
		pitchInput.ResetChanged();

		int32_t pitch = pitchInput.GetValue() + static_cast<int32_t>(shiftInput.GetValue() * Config::pitchPerSemitone);
		
		if (pitch > Config::unsignedMax)
			pitch = Config::unsignedMax;
		else if (pitch < 0)
			pitch = 0;

		_unsignedOutputs[Pin::PitchShift::UnsignedOutput::Pitch].SetValue(pitch);
	}
}
