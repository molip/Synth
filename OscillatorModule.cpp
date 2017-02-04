#include "OscillatorModule.h"

OscillatorModule::OscillatorModule()
{
	_unsignedInputs.SetSize(Pin::Oscillator::UnsignedInput::_Count);
	_signedOutputs.SetSize(Pin::Oscillator::SignedOutput::_Count);
}

uint16_t OscillatorModule::PitchToPhaseDelta(unsigned_t pitch) 
{
	int8_t midiNote = pitch >> 9; // 7 bit;
	int octave = midiNote / 12;
	int pitchIndex = midiNote % 12;
	
	float freq = Config::freqs[pitchIndex];
	for (int i = 0; i < octave; ++i)
		freq = freq + freq;

	//Serial.print("freq: "); Serial.println(freq);

	return uint16_t(freq * Config::freqToDeltaScale);
}

void OscillatorModule::Update()
{
	const UnsignedInput& levelInput = _unsignedInputs[Pin::Oscillator::UnsignedInput::Level];
	const UnsignedInput& pitchInput = _unsignedInputs[Pin::Oscillator::UnsignedInput::Pitch];
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
		_phaseDelta = PitchToPhaseDelta(pitchInput.GetValue());
	}

	_phase += _phaseDelta;

	uint16_t output = ((_phase & 0x8000) ? 0xffff - _phase : _phase) << 1; // Triangle.

	//Serial.println(output);

	signalOutput.SetValue(((output - 0x8000) * level) >> 16); // Signed.
}
