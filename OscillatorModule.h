#pragma once 
#include "Module.h"

class OscillatorModule : public Module
{
public:
	virtual void Update();
	UnsignedInput _levelInput, _pitchInput;
	SignedOutput _output;
private:
	uint16_t PitchToPhaseDelta(unsigned_t pitch);

	uint16_t _phase = 0; 		// [0, 0xffff]
	uint16_t _phaseDelta = 0;
};
