#pragma once 
#include "Module.h"

namespace Engine
{
class PolyMixerModule : public Module
{
public:
	PolyMixerModule(int polyphony);
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }
private:
	float _scale;
};
}