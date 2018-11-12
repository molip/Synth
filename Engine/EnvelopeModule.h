#pragma once 
#include "Module.h"

namespace Engine
{
class EnvelopeModule : public Module
{
public:
	EnvelopeModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }

private:
	enum class Stage { Off, Attack, Decay, Sustain, Release };

	uint32_t _attackDelta = 0;
    uint32_t _decayDelta = 0;
    uint32_t _releaseDelta = 0;
    uint32_t _sustainLevel = 0;
    uint32_t _level = 0;
    Stage _stage = Stage::Off;
};
}