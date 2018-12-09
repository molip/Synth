#include "Config.h"

using namespace Engine;

float Config::uint16ToFloat;
float Config::uint16ToFloat2;
float Config::uint32ToFloat;
float Config::analogInToFloat;

float Config::freqs[Config::pitchStepsPerOctave];
float Config::freqFactors[12];

const uint32_t Config::sampleRate = 48000;
const uint32_t Config::sampleRateMS = 48;
float Config::freqToDeltaScale = (1 << 16) / (float)sampleRate;
