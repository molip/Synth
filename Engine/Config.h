#pragma once 
#include <arduino.h>

class Config
{
public:
	static void Init()
	{
		const float baseFreq = 16.35; // C0
		const float pitchMult = ::pow(2, 1.0 / pitchesPerOctave);
		float freq = baseFreq;
		for (int i = 0; i < pitchesPerOctave; ++i)
		{
			freqs[i] = freq;
			freq *= pitchMult;
		}
	}

	using value_t = uint16_t;

	static const int pitchesPerOctave = 12 * 16;
	static float freqs[pitchesPerOctave];

	static const uint32_t sampleRate, sampleRateMS;
	static float freqToDeltaScale;
	
	static const value_t valueMax;
};
