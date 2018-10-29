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

	float level = levelInput.GetValue();
	if (level == 0)
	{
		signalOutput.SetValue(0);
		_phase = 0;
		return;
	}

	level *= Config::divUnsignedMax; // [0, 1]

	if (pitchInput.HasChanged())
	{
		pitchInput.ResetChanged();
		_phaseDelta = ::PitchToPhaseDelta(pitchInput.GetValue());
	}

	_phase += _phaseDelta;
	
	uint16_t phase = _phase + static_cast<int16_t>(_signedInputs[Pin::Oscillator::SignedInput::PhaseMod].GetValue() * 0x7fff);
	uint16_t output = ::SampleWaveform(_unsignedInputs[Pin::Oscillator::UnsignedInput::Waveform].GetValue(), phase, 0x8000);

	signalOutput.SetValue(UnsignedToSigned(output) * level); // Signed.
}
