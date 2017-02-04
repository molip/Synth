#include "PolyMixerModule.h"

PolyMixerModule::PolyMixerModule()
{
	_signedPolyInputs.SetSize(Pin::PolyMixer::SignedPolyInput::_Count);
	_signedOutputs.SetSize(Pin::PolyMixer::SignedOutput::_Count);
}

void PolyMixerModule::Update()
{
	SignedPolyInput& polyInput = _signedPolyInputs[Pin::PolyMixer::SignedPolyInput::Signal];
	uint32_t total = 0;
	int count = polyInput.GetSize();
	for (int i = 0; i < count; ++i)
		total += polyInput[i].GetValue();

	_signedOutputs[Pin::PolyMixer::SignedOutput::Signal].SetValue(total / count);
}
