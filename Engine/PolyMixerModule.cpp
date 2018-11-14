#include "PolyMixerModule.h"

using namespace Engine;

PolyMixerModule::PolyMixerModule(int polyphony)
{
	_multiInputs.SetSize(Pin::PolyMixer::MultiInput::_Count);
	_outputs.SetSize(Pin::PolyMixer::Output::_Count);

	_scale = 1.0f / polyphony;
}

void PolyMixerModule::Update()
{
	MultiInput& multiInput = _multiInputs[Pin::PolyMixer::MultiInput::Signal];
	float total = 0;
	int count = multiInput.GetSize();
	for (int i = 0; i < count; ++i)
		total += multiInput[i].GetValue();

	_outputs[Pin::PolyMixer::Output::Signal].SetValue(total * _scale);
}
