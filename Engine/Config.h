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

		pitchPerSemitone = 0xffff / 120.0f;
		pitchPerOctave = pitchPerSemitone * 12;
		pitchStepsPerPitch = 16 * 120.0f / 0xffff;

		divUnsignedMax = 1.0f / unsignedMax;
		divUnsignedMax2 = 1.0f / unsignedMax2;
	}

	using unsigned_t = uint16_t;
	using signed_t = float;

	static const int pitchStepsPerOctave = 12 * 16;

	static float pitchPerSemitone;
	static float pitchPerOctave;
	static float pitchStepsPerPitch;

	static float divUnsignedMax;
	static float divUnsignedMax2;

	static float freqs[pitchStepsPerOctave];
	static float freqFactors[12];

	static const uint32_t sampleRate, sampleRateMS;
	static float freqToDeltaScale;
	
	static const unsigned_t unsignedMax;
	static const float unsignedMax2;
};
}