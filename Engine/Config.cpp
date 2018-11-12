#include "Config.h"

using namespace Engine;

float Config::pitchPerSemitone;
float Config::pitchPerOctave;
float Config::pitchStepsPerPitch;

float Config::uint16ToFloat;
float Config::uint32ToFloat;

float Config::freqs[Config::pitchStepsPerOctave];
float Config::freqFactors[12];

const uint32_t Config::sampleRate = 50000;
const uint32_t Config::sampleRateMS = 50;
float Config::freqToDeltaScale = (1 << 16) / (float)sampleRate;
