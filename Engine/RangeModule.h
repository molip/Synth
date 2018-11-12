#pragma once 
#include "Module.h"

namespace Engine
{
class RangeModule : public Module
{
public:
	RangeModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }
};
}