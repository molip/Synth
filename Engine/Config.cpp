#include "Config.h"

float Config::pitchPerSemitone;
float Config::pitchPerOctave;
float Config::pitchStepsPerPitch;

float Config::freqs[Config::pitchStepsPerOctave];
float Config::freqFactors[12];

const uint32_t Config::sampleRate = 50000;
const uint32_t Config::sampleRateMS = 50;
float Config::freqToDeltaScale = (1 << 16) / (float)sampleRate;

const Config::value_t Config::valueMax = 0xffff;

