#pragma once 
#include "Module.h"
#include "Util.h"

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
	SampleWaveformContext _ctx;
	bool _positive = false;
	bool _hold = false;
	float _lastLevel = 0;
};
}