#include "Config.h"

using namespace Engine;

float Config::uint16ToFloat;
float Config::uint16ToFloat2;
float Config::uint32ToFloat;
float Config::analogInToFloat;

float Config::freqs[Config::pitchStepsPerOctave];
float Config::freqFactors[12];

uint32_t Config::sampleRate;
uint32_t Config::sampleRateMS;
float Config::freqToDeltaScale;
