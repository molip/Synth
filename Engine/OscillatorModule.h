#pragma once 
#include "Module.h"

namespace Engine
{
class OscillatorModule : public Module
{
public:
	OscillatorModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }

private:
	uint16_t _phase = 0; 		// [0, 0xffff]
	uint16_t _phaseDelta = 0;
};
}