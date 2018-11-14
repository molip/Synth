#include "MixerModule.h"

using namespace Engine;

MixerModule::MixerModule()
{
	_inputs.SetSize(Pin::Mixer::Input::_Count);
	_outputs.SetSize(Pin::Mixer::Output::_Count);
	_activeInputs.Reserve(_inputs.GetSize());
}

void MixerModule::Update()
{
	if (!_initialised)
	{
		for (int i = 0; i < _inputs.GetSize(); ++i)
			if (_inputs[i].IsConnected())
				_activeInputs.Push(&_inputs[i]);

		_scale = 1.0f / _activeInputs.GetSize();
		_initialised = true;
	}

	const int count = _activeInputs.GetSize();
	if (count == 0)
		return;

	float total = 0;
	for (int i = 0; i < count; ++i)
		total += _activeInputs[i]->GetValue();

	_outputs[Pin::Mixer::Output::Signal].SetValue(total * _scale);
}
