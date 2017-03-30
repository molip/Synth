#include "PitchShiftModule.h"
#include "Util.h"

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

		int32_t pitch = pitchInput.GetValue() + shiftInput.GetValue() * Config::pitchPerSemitone;
		
		if (pitch > Config::valueMax)
			pitch = Config::valueMax;
		else if (pitch < 0)
			pitch = 0;

		_unsignedOutputs[Pin::PitchShift::UnsignedOutput::Pitch].SetValue(pitch);
	}
}
