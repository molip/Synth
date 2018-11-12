#include "PitchShiftModule.h"
#include "Util.h"

using namespace Engine;

PitchShiftModule::PitchShiftModule()
{
	_unsignedInputs.SetSize(Pin::PitchShift::UnsignedInput::_Count);
	_unsignedOutputs.SetSize(Pin::PitchShift::UnsignedOutput::_Count);
}

void PitchShiftModule::Update()
{
	UnsignedInput& shiftInput = _unsignedInputs[Pin::PitchShift::UnsignedInput::Shift];
	UnsignedInput& pitchInput = _unsignedInputs[Pin::PitchShift::UnsignedInput::Pitch];
	
	if (shiftInput.HasChanged() || pitchInput.HasChanged())
	{
		shiftInput.ResetChanged();
		pitchInput.ResetChanged();

		const int16_t shift = static_cast<int16_t>(shiftInput.GetValue()) - 128; // See Exporter::WriteValues.
		int32_t pitch = pitchInput.GetValue() + static_cast<int32_t>(shift * Config::pitchPerSemitone);
		
		if (pitch > 0xffff)
			pitch = 0xffff;
		else if (pitch < 0)
			pitch = 0;

		_unsignedOutputs[Pin::PitchShift::UnsignedOutput::Pitch].SetValue(pitch);
	}
}
