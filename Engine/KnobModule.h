#pragma once 
#include "Module.h"

namespace Engine
{
class KnobModule : public Module
{
public:
	KnobModule();
	void SetValue(int iActive, uint16_t val);
	void UpdateRemote();
	int GetActiveCount() const { return _activeInputs.GetSize(); }

private:
	virtual void Initialise() override;
	void UpdateValue(int iActive);

	const int KnobCount = Pin::Knob::Input::_Count;
	Array <uint16_t> _values;
	Array<Input*> _activeInputs;
	Array<Output*> _activeOutputs;
};
}