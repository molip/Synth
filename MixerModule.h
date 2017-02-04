#pragma once 
#include "Module.h"

class MixerModule : public Module
{
public:
	MixerModule(int inputCount);
	virtual ~MixerModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }

	int _inputCount;
	SignedInput* _inputs;
	SignedOutput _ouput;
};
