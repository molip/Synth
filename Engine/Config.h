#pragma once 
#include "Platform.h"

namespace Engine
{
class Config
{
public:
	static void Init()
	{
		const float baseFreq = 8.175799f;
		const float pitchMult = ::pow(2.0f, 1.0f / pitchStepsPerOctave);
		float freq = baseFreq;
		for (int i = 0; i < pitchStepsPerOctave; ++i)
		{
			freqs[i] = freq;
			freq *= pitchMult;
		}

		const float pitchMult12 = ::pow(2.0f, 1.0f / 12);
		freq = 1;
		for (int i = 0; i < 12; ++i)
		{
			freqFactors[i] = freq;
			freq *= pitchMult12;
		}

		uint16ToFloat = 1.0f / 0xffff;
		uint16ToFloat2 = 2.0f / 0xffff;
		uint32ToFloat = 1.0f / 0xffffffff;
		analogInToFloat = 1.0f / 0x3ff;
	}

	static const int pitchStepsPerSemitone = 16;
	static const int pitchStepsPerOctave = 12 * pitchStepsPerSemitone;
	static const int midiNoteCount = 120; 

	static float pitchStepsPerPitch;

	static float uint16ToFloat;
	static float uint16ToFloat2;
	static float uint32ToFloat;
	static float analogInToFloat;

	static float freqs[pitchStepsPerOctave];
	static float freqFactors[12];

	static const uint32_t sampleRate, sampleRateMS;
	static float freqToDeltaScale;
};
}