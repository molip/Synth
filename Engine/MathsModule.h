#pragma once 
#include "Module.h"

namespace Engine
{
class MathsModule : public Module
{
public:
	MathsModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }
};
}