#pragma once 
#include "Module.h"

class OutputModule : public Module
{
public:
	OutputModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }
};
