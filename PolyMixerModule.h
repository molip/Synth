#pragma once 
#include "Module.h"

class PolyMixerModule : public Module
{
public:
	PolyMixerModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }
};
