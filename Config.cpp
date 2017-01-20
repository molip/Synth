#include "Config.h"

float Config::pitchMult = 1.059463094359;
float Config::freqs[12];

const uint32_t Config::sampleRate = 50000;
float Config::freqToDeltaScale = (1 << 16) / (float)sampleRate;

const Config::value_t Config::valueMax = 0xffff;

