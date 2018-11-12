#pragma once 
#include "Module.h"

namespace Engine
{
class PitchShiftModule : public Module
{
public:
	PitchShiftModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }

private:
	float _multiplier = 1;
};
}