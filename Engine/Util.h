#pragma once 

#include <arduino.h>
#include "Config.h"
#include "Sine.h"

inline extern uint16_t SampleWaveform(byte waveform, uint16_t phase, uint16_t duty)
{
	uint16_t output = 0;
	switch (waveform)
	{
	case 0: // Sawtooth. 
		output = phase;
		break;
	case 1: // Triangle.
		output = ((phase & 0x8000) ? 0xffff - phase : phase) << 1;
		break;
	case 2: // Square.
		output = phase < duty ? 0xffff : 0;
		break;
	case 3: // Sine.
		output = pgm_read_byte_near(SineTable + (phase >> 5)) << 8;
		break;
	}
	return output;
}

inline extern uint16_t PitchToPhaseDelta(uint16_t pitch) // pitch: midi note << 4 (11 bit)
{
	uint16_t pitchIndex = pitch >> 5; // 11 bit;
	int octave = pitchIndex / Config::pitchesPerOctave;
	int freqIndex = pitchIndex % Config::pitchesPerOctave;

	float freq = Config::freqs[freqIndex];
	for (int i = 0; i < octave; ++i)
		freq = freq + freq;

	return uint16_t(freq * Config::freqToDeltaScale);
}
