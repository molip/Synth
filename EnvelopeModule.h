#pragma once 
#include "Module.h"

class EnvelopeModule : public Module
{
public:
	virtual void Update();
	UnsignedInput _gateInput; // Bool.
	UnsignedInput _attackInput, _decayInput, _releaseInput; // ms.
	UnsignedInput _sustainInput; // Level.
	UnsignedOutput _output; // Level.

private:
	enum class Stage { Off, Attack, Decay, Sustain, Release };

	uint32_t _attackDelta = 0;
    uint32_t _decayDelta = 0;
    uint32_t _releaseDelta = 0;
    uint32_t _sustainLevel = 0;
    uint32_t _level = 0;
    Stage _stage = Stage::Off;
};
