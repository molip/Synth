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
		output = phase + 0x8000;
		break;
	case 1: // Triangle.
		phase += 0x4000;
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

// Converts midi note [0, 127] to 16 bit.
inline extern uint16_t MidiNoteToPitch(byte midiNote)
{
	return (midiNote < 120 ? midiNote : 120) * Config::pitchPerSemitone;
}

inline extern float GetPitchMultiplier(int16_t semitones)
{
	if (semitones == 0)
		return 1;

	int octaves = semitones / 12;
	int index = semitones % 12;

	float result, octaveMult;
	if (semitones > 0)
	{
		result = Config::freqFactors[index];
		octaveMult = 2;
	}
	else
	{
		result = Config::freqFactors[12 - index];
		octaveMult = 0.5;
		octaves = 1 - octaves;
	}

	for (int i = 0; i < octaves; ++i)
		result *= octaveMult;

	Serial.println(index);
	Serial.println(octaves);

	Serial.println(result);
	return result;
}

inline extern uint16_t PitchToPhaseDelta(uint16_t pitch) // pitch: midi note << 4 (11 bit)
{
	uint16_t pitchIndex = pitch * Config::pitchStepsPerPitch;

	int octave = pitchIndex / Config::pitchStepsPerOctave;
	int freqIndex = pitchIndex % Config::pitchStepsPerOctave;

	float freq = Config::freqs[freqIndex];
	for (int i = 0; i < octave; ++i)
		freq = freq + freq;

	return uint16_t(freq * Config::freqToDeltaScale);
}
