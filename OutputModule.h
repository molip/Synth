#pragma once 
#include "Module.h"

class OutputModule : public Module
{
public:
	virtual void Update() override;
	SignedInput _input;
};
