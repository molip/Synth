#pragma once 
#include "Module.h"

class LFOModule : public Module
{
public:
	LFOModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }

private:
	uint16_t _phase = 0; 		// [0, 0xffff]
	uint16_t _phaseDelta = 0;
	uint16_t _divide = 0;
};