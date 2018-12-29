#pragma once 
#include "Module.h"

namespace Engine
{
class NoiseModule : public Module
{
public:
	NoiseModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }

private:
	uint32_t _phase = 0;
	uint16_t _phaseDelta = 0;
};
}