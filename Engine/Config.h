#pragma once 
#include <arduino.h>

class Config
{
public:
	static void Init()
	{
		const float baseFreq = 16.35; // C0
		const float pitchMult = ::pow(2, 1.0 / pitchStepsPerOctave);
		float freq = baseFreq;
		for (int i = 0; i < pitchStepsPerOctave; ++i)
		{
			freqs[i] = freq;
			freq *= pitchMult;
		}

		const float pitchMult12 = ::pow(2, 1.0 / 12);
		freq = 1;
		for (int i = 0; i < 12; ++i)
		{
			freqFactors[i] = freq;
			freq *= pitchMult12;
		}

		pitchPerSemitone = 0xffff / 120.0;
		pitchPerOctave = pitchPerSemitone * 12;
		pitchStepsPerPitch = 16 * 120.0 / 0xffff;
	}

	using value_t = uint16_t;

	static const int pitchStepsPerOctave = 12 * 16;

	static float pitchPerSemitone;
	static float pitchPerOctave;
	static float pitchStepsPerPitch;

	static float freqs[pitchStepsPerOctave];
	static float freqFactors[12];

	static const uint32_t sampleRate, sampleRateMS;
	static float freqToDeltaScale;
	
	static const value_t valueMax;
};
