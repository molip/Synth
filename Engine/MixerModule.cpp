#include "MixerModule.h"

using namespace Engine;

MixerModule::MixerModule()
{
	_multiInputs.SetSize(Pin::Mixer::Input::_Count);
	_multiOutputs.SetSize(Pin::Mixer::Output::_Count);
	_activeInputs.Reserve(_multiInputs.GetSize());
}

void MixerModule::Update()
{
	for (int i = 0; i < _multiInputs[0].GetSize(); ++i)
		UpdateInstance(i);
}

void MixerModule::UpdateInstance(int index)
{
	if (!_initialised)
	{
		for (int i = 0; i < _multiInputs.GetSize(); ++i)
			if (_multiInputs[i][0].IsConnected())
				_activeInputs.Push(&_multiInputs[i]);

		_scale = 1.0f / _activeInputs.GetSize();
		_initialised = true;
	}

	const int count = _activeInputs.GetSize();
	if (count == 0)
		return;

	float total = 0;
	for (int i = 0; i < count; ++i)
		total += (*_activeInputs[i])[index].GetValue();

	_multiOutputs[Pin::Mixer::Output::Signal][index].SetValue(total * _scale);
}
