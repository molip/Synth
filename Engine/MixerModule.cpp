#include "MixerModule.h"

MixerModule::MixerModule()
{
	_signedInputs.SetSize(Pin::Mixer::SignedInput::_Count);
	_signedOutputs.SetSize(Pin::Mixer::SignedOutput::_Count);
	_activeInputs.Reserve(_signedInputs.GetSize());
}

void MixerModule::Update()
{
	if (!_initialised)
	{
		for (int i = 0; i < _signedInputs.GetSize(); ++i)
			if (_signedInputs[i].IsConnected())
				_activeInputs.Push(&_signedInputs[i]);

		_initialised = true;
	}

	const int count = _activeInputs.GetSize();
	if (count == 0)
		return;

	int32_t total = 0;
	for (int i = 0; i < count; ++i)
		total += _activeInputs[i]->GetValue();

	_signedOutputs[Pin::Mixer::SignedOutput::Signal].SetValue(total / count);
}
