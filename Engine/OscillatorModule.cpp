#include "OscillatorModule.h"
#include "Sine.h"

OscillatorModule::OscillatorModule()
{
	_unsignedInputs.SetSize(Pin::Oscillator::UnsignedInput::_Count);
	_signedInputs.SetSize(Pin::Oscillator::SignedInput::_Count);
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
		_phaseDelta = PitchToPhaseDelta(pitchInput.GetValue());
	}

	_phase += _phaseDelta;
	
	uint16_t phase = _phase + _signedInputs[Pin::Oscillator::SignedInput::PhaseMod].GetValue();

	uint16_t output = 0;
	switch (_unsignedInputs[Pin::Oscillator::UnsignedInput::Waveform].GetValue())
	{
	case 0: // Sawtooth. 
		output = phase;
		break;
	case 1: // Triangle.
		output = ((phase & 0x8000) ? 0xffff - phase : phase) << 1;
		break;
	case 2: // Square.
		output = phase > 0x8000 ? 0xffff : 0;
		break;
	case 3: // Sine.
		output = pgm_read_byte_near(SineTable + (phase >> 5)) << 8;
		break;
	}

	//Serial.println(output);

	signalOutput.SetValue(((output - 0x8000) * level) >> 16); // Signed.
}
