#pragma once 
#include "Module.h"

namespace Engine
{
class DelayModule : public Module
{
public:
	DelayModule();
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }

private:
	static Array<float> _buffer; // TODO: Allow polyphony.
	float _feedback = 0;
	uint32_t _current = 0;
	uint16_t _period = 0;
	bool _clear = true;
};
}