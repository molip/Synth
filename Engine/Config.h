#pragma once 
#include "Platform.h"

namespace Engine
{
class Config
{
public:
	static void Init()
	{
		const float baseFreq = 16.35f; // C0
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

		pitchPerSemitone = 1.0f / midiNoteCount;
		pitchPerOctave = pitchPerSemitone * 12;

		uint16ToFloat = 1.0f / 0xffff;
		uint32ToFloat = 1.0f / 0xffffffff;
	}

	static const int pitchStepsPerSemitone = 16;
	static const int pitchStepsPerOctave = 12 * pitchStepsPerSemitone;
	static const int midiNoteCount = 120; 

	static float pitchPerSemitone;
	static float pitchPerOctave;
	static float pitchStepsPerPitch;

	static float uint16ToFloat;
	static float uint32ToFloat;

	static float freqs[pitchStepsPerOctave];
	static float freqFactors[12];

	static const uint32_t sampleRate, sampleRateMS;
	static float freqToDeltaScale;
};
}