#pragma once 
#include "Module.h"

namespace Engine
{
class MixerModule : public Module
{
public:
	MixerModule();
	virtual void Initialise() override;
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }
private:
	Array<Input*> _activeInputs;
	float _scale = 0;
};
}