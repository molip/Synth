#pragma once 
#include "Module.h"

namespace Engine
{
class KnobModule : public Module
{
public:
	KnobModule();
	void SetValue(uint16_t val);
	int GetIndex() const;
	void UpdateRemote();

private:
	virtual void Initialise() override;
	void UpdateValue();

	uint16_t _value = 0;
};
}