#pragma once 
#include "Module.h"

class RangeModule : public Module
{
public:
	RangeModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }
};
