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
	Array<signed_t> _buffer;
	float _feedback = 0;
	uint32_t _current = 0;
	unsigned_t _period = 0;
	bool _clear = true;
};
}