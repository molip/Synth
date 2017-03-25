#include "PitchShiftModule.h"

PitchShiftModule::PitchShiftModule()
{
	_signedInputs.SetSize(Pin::PitchShift::SignedInput::_Count);
	_unsignedInputs.SetSize(Pin::PitchShift::UnsignedInput::_Count);
	_unsignedOutputs.SetSize(Pin::PitchShift::UnsignedOutput::_Count);
}

void PitchShiftModule::Update()
{
	int16_t pitch = _unsignedInputs[Pin::PitchShift::UnsignedInput::Pitch].GetValue() >> 9;

	pitch += _signedInputs[Pin::PitchShift::SignedInput::Shift].GetValue();
	if (pitch > 127)
		pitch = 127;
	else if (pitch < 0)
		pitch = 0;

	_unsignedOutputs[Pin::PitchShift::UnsignedOutput::Pitch].SetValue(pitch << 9);
}
