#include "PolyMixerModule.h"

using namespace Engine;

PolyMixerModule::PolyMixerModule(int polyphony)
{
	_signedMultiInputs.SetSize(Pin::PolyMixer::SignedMultiInput::_Count);
	_signedOutputs.SetSize(Pin::PolyMixer::SignedOutput::_Count);

	_scale = 1.0f / polyphony;
}

void PolyMixerModule::Update()
{
	SignedMultiInput& multiInput = _signedMultiInputs[Pin::PolyMixer::SignedMultiInput::Signal];
	signed_t total = 0;
	int count = multiInput.GetSize();
	for (int i = 0; i < count; ++i)
		total += multiInput[i].GetValue();

	_signedOutputs[Pin::PolyMixer::SignedOutput::Signal].SetValue(signed_t(total * _scale));
}
