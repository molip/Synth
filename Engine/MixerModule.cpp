#include "MixerModule.h"

using namespace Engine;

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

		_scale = 1.0f / _activeInputs.GetSize();
		_initialised = true;
	}

	const int count = _activeInputs.GetSize();
	if (count == 0)
		return;

	signed_t total = 0;
	for (int i = 0; i < count; ++i)
		total += _activeInputs[i]->GetValue();

	_signedOutputs[Pin::Mixer::SignedOutput::Signal].SetValue(total * _scale);
}
