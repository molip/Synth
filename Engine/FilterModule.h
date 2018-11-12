#pragma once 
#include "Module.h"

namespace Engine
{
class FilterModule : public Module
{
public:
	FilterModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }

private:
	float _p = 0;
	float _q = 0;
	float _f = 0;
	float _b0 = 0;
	float _b1 = 0;
	float _b2 = 0;
	float _b3 = 0;
	float _b4 = 0;
};
}