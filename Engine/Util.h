#pragma once 

#include "Config.h"
#include "Sine.h"

inline extern uint16_t SampleWaveform(byte waveform, uint16_t phase, uint16_t duty)
{
	uint16_t output = 0;
	switch (waveform)
	{
	case 0: // Sawtooth. 
		output = phase + 0x8000;
		break;
	case 1: // Triangle.
		phase += 0x4000;
		output = ((phase & 0x8000) ? 0xffff - phase : phase) << 1;
		break;
	case 2: // Square.
		output = phase < uint16_t(~duty) ? 0 : 0xffff;
		break;
	case 3: // Sine.
		output = *(SineTable + (phase >> 5)) << 8;
		break;
	}
	return output;
}

// Converts midi note [0, 127] to 16 bit.
inline extern uint16_t MidiNoteToPitch(byte midiNote)
{
	return static_cast<uint16_t>((midiNote < 120 ? midiNote : 120) * Config::pitchPerSemitone);
}

inline extern uint16_t PitchToPhaseDelta(uint16_t pitch) // pitch: midi note << 4 (11 bit)
{
	uint16_t pitchIndex = static_cast<uint16_t>(pitch * Config::pitchStepsPerPitch);

	int octave = pitchIndex / Config::pitchStepsPerOctave;
	int freqIndex = pitchIndex % Config::pitchStepsPerOctave;

	float freq = Config::freqs[freqIndex];
	for (int i = 0; i < octave; ++i)
		freq = freq + freq;

	return uint16_t(freq * Config::freqToDeltaScale);
}

inline extern float ClipSigned(float val)
{
	if (val > 1)
		val = 1;
	else if (val < -1)
		val = -1;

	return val;
}

inline extern Config::signed_t UnsignedToSigned(Config::unsigned_t val)
{
	return val * Config::divUnsignedMax2 - 1;
}

inline extern Config::unsigned_t SignedToUnsigned(Config::signed_t val)
{
	return Config::unsigned_t((val + 1) * Config::unsignedMax2);
}