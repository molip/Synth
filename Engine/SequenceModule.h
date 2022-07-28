#pragma once 
#include "Array.h"
#include "Module.h"
#include "Util.h"

namespace Engine
{
class SequenceModule : public Module
{
public:
	SequenceModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }

private:
	uint16_t _phase = 0; // [0, 0xffff]
	uint16_t _phaseDelta = 0;
	uint16_t _divide = 0;
	uint16_t _index = 0;
};
}