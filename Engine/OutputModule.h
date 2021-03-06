#pragma once 
#include "Module.h"

namespace Engine
{
class OutputModule : public Module
{
public:
	OutputModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }

	float GetLevel() const { return _result; }

private:
	float _result = 0;
};
}