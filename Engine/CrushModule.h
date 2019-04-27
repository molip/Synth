#pragma once 
#include "Module.h"

namespace Engine
{
class CrushModule : public Module
{
public:
	CrushModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }

private:
	float _signal = 0;
	float _phase = 0;
	float _phaseDelta = 0;
	float _quantum = 0;
	float _quantumInv = 0;
};
}