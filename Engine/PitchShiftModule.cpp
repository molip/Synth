#include "PitchShiftModule.h"
#include "Util.h"

using namespace Engine;

PitchShiftModule::PitchShiftModule()
{
	_signedInputs.SetSize(Pin::PitchShift::SignedInput::_Count);
	_signedOutputs.SetSize(Pin::PitchShift::SignedOutput::_Count);
}

void PitchShiftModule::Update()
{
	SignedInput& shiftInput = _signedInputs[Pin::PitchShift::SignedInput::Shift];
	SignedInput& pitchInput = _signedInputs[Pin::PitchShift::SignedInput::Pitch];
	
	if (shiftInput.HasChanged() || pitchInput.HasChanged())
	{
		shiftInput.ResetChanged();
		pitchInput.ResetChanged();

		const int16_t shift = static_cast<int16_t>(shiftInput.GetValue()) - 128; // See Exporter::WriteValues.
		float pitch = pitchInput.GetValue() + shift * Config::pitchPerSemitone;
		
		_signedOutputs[Pin::PitchShift::SignedOutput::Pitch].SetValue(pitch);
	}
}
