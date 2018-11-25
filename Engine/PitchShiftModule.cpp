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
	
	const float pitch = pitchInput.GetValue() + shiftInput.GetValue();
	_outputs[Pin::PitchShift::Output::Pitch].SetValue(pitch);
}
