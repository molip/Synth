#pragma once 
#include <arduino.h>

class Config
{
public:
	static void Init()
	{
		const float baseFreq = 16.35; // C0
	
		float freq = baseFreq;
		for (int i = 0; i < 12; ++i)
		{
			freqs[i] = freq;
			freq *= pitchMult;
		}
	}

	using value_t = uint16_t;

	static float pitchMult;
	static float freqs[12];

	static const uint32_t sampleRate, sampleRateMS;
	static float freqToDeltaScale;
	
	static const value_t valueMax;
};
