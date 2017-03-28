#include "OscillatorModule.h"
#include "Util.h"

OscillatorModule::OscillatorModule()
{
	_unsignedInputs.SetSize(Pin::Oscillator::UnsignedInput::_Count);
	_signedInputs.SetSize(Pin::Oscillator::SignedInput::_Count);
	_signedOutputs.SetSize(Pin::Oscillator::SignedOutput::_Count);
}

void OscillatorModule::Update()
{
	UnsignedInput& levelInput = _unsignedInputs[Pin::Oscillator::UnsignedInput::Level];
	UnsignedInput& pitchInput = _unsignedInputs[Pin::Oscillator::UnsignedInput::Pitch];
	SignedOutput& signalOutput = _signedOutputs[Pin::Oscillator::SignedOutput::Signal];

	unsigned_t level = levelInput.GetValue();
	if (level == 0)
	{
		signalOutput.SetValue(0);
		_phase = 0;
		return;
	}

	if (pitchInput.HasChanged())
	{
		pitchInput.ResetChanged();
		_phaseDelta = ::PitchToPhaseDelta(pitchInput.GetValue());
	}

	_phase += _phaseDelta;
	
	uint16_t phase = _phase + _signedInputs[Pin::Oscillator::SignedInput::PhaseMod].GetValue();
	uint16_t output = ::SampleWaveform(_unsignedInputs[Pin::Oscillator::UnsignedInput::Waveform].GetValue(), phase, 0x8000);

	signalOutput.SetValue(((output - 0x8000) * level) >> 16); // Signed.
}
