#pragma once 
#include "Module.h"

namespace Engine
{
class MixerModule : public Module
{
public:
	MixerModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }
private:
	Array<SignedInput*> _activeInputs;
	bool _initialised = false;
	float _scale = 0;
};
}