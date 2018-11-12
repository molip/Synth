#include "OscillatorModule.h"
#include "Util.h"

using namespace Engine;

OscillatorModule::OscillatorModule()
{
	_unsignedInputs.SetSize(Pin::Oscillator::UnsignedInput::_Count);
	_signedInputs.SetSize(Pin::Oscillator::SignedInput::_Count);
	_signedOutputs.SetSize(Pin::Oscillator::SignedOutput::_Count);
}

void OscillatorModule::Update()
{
	SignedInput& levelInput = _signedInputs[Pin::Oscillator::SignedInput::Level];
	UnsignedInput& pitchInput = _unsignedInputs[Pin::Oscillator::UnsignedInput::Pitch];
	SignedOutput& signalOutput = _signedOutputs[Pin::Oscillator::SignedOutput::Signal];

	const float level = levelInput.GetValue();

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
	
	uint16_t phase = _phase + static_cast<int16_t>(_signedInputs[Pin::Oscillator::SignedInput::PhaseMod].GetValue() * 0x7fff);
	const byte waveform = static_cast<byte>(_unsignedInputs[Pin::Oscillator::UnsignedInput::Waveform].GetValue());
	uint16_t output = ::SampleWaveform(waveform, phase, 0x8000);

	signalOutput.SetValue((output * Config::uint16ToFloat - 0.5f) * level);
}
