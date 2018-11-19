#pragma once 
#include "Module.h"

namespace Engine
{
class MultiplyModule : public Module
{
public:
	MultiplyModule();
	virtual void Initialise() override;
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }
private:
	Array<Input*> _activeInputs;
};
}