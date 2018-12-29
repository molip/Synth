#pragma once 

#include "Config.h"
#include "Sine.h"

namespace Engine
{

struct SampleWaveformContext 
{
	bool low = false;
	uint16_t lastRandom = 0;
};

inline extern uint16_t SampleWaveform(byte waveform, uint16_t phase, uint16_t duty, SampleWaveformContext* ctx)
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
		output = *(SineTable + (phase >> 3));
		break;
	case 4: // Noise.
	{
		bool low = (phase & 0x8000) == 0;
		if (low && !ctx->low)
			ctx->lastRandom = ::rand() << 1;

		ctx->low = low;
		output = ctx->lastRandom;
		break;
	}
	}
	return output;
}

inline extern float ClipBipolar(float val)
{
	if (val > 1.0f)
		val = 1.0f;
	else if (val < -1.0f)
		val = -1.0f;

	return val;
}

inline extern float ClipUnipolar(float val)
{
	if (val > 1.0f)
		val = 1.0f;
	else if (val < 0)
		val = 0;

	return val;
}

inline extern uint16_t PitchToPhaseDelta(float pitch) 
{
	pitch = pitch < 0 ? 0 : pitch > 127 ? 127 : pitch;
	uint16_t pitchIndex = static_cast<uint16_t>(pitch * Config::pitchStepsPerSemitone);

	int octave = pitchIndex / Config::pitchStepsPerOctave;
	int freqIndex = pitchIndex % Config::pitchStepsPerOctave;

	float freq = Config::freqs[freqIndex];
	for (int i = 0; i < octave; ++i)
		freq = freq + freq;

	return uint16_t(freq * Config::freqToDeltaScale);
}

inline extern uint16_t FloatToFixed16(float val)
{
	return static_cast<uint16_t>(val * 0xffff);
}

inline extern uint32_t FloatToFixed32(float val)
{
	const uint32_t max = 0xffffffff;
	const float overflow = 0.9999999f; // Prevent integer overflow.
	return val > overflow ? max : static_cast<uint32_t>(val * max);
}
}
