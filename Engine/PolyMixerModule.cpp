#include "PolyMixerModule.h"

PolyMixerModule::PolyMixerModule()
{
	_signedMultiInputs.SetSize(Pin::PolyMixer::SignedMultiInput::_Count);
	_signedOutputs.SetSize(Pin::PolyMixer::SignedOutput::_Count);
}

void PolyMixerModule::Update()
{
	SignedMultiInput& multiInput = _signedMultiInputs[Pin::PolyMixer::SignedMultiInput::Signal];
	uint32_t total = 0;
	int count = multiInput.GetSize();
	for (int i = 0; i < count; ++i)
		total += multiInput[i].GetValue();

	_signedOutputs[Pin::PolyMixer::SignedOutput::Signal].SetValue(total / count);
}
