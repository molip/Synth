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
	Array<Input*> _activeInputs;
	bool _initialised = false;
	float _scale = 0;
};
}