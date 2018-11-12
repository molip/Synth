#pragma once 

#include "Config.h"
#include "Sine.h"

namespace Engine
{
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
	if (val > 0.5f)
		val = 0.5f;
	else if (val < -0.5f)
		val = -0.5f;

	return val;
}

inline extern uint16_t FloatToFixed16(float val)
{
	return static_cast<uint16_t>(val * 0xffff);
}

inline extern uint32_t FloatToFixed32(float val)
{
	return static_cast<uint32_t>(val * 0xffffffff);
}
}
