#pragma once 
#include "Module.h"

class MixerModule : public Module
{
public:
	MixerModule(int inputCount);
	virtual ~MixerModule();
	virtual void Update() override;

	int _inputCount;
	SignedInput* _inputs;
	SignedOutput _ouput;
};
