#include "MultiplyModule.h"

using namespace Engine;

MultiplyModule::MultiplyModule()
{
	_inputs.SetSize(Pin::Mixer::Input::_Count);
	_outputs.SetSize(Pin::Mixer::Output::_Count);
	_activeInputs.Reserve(_inputs.GetSize());
}

void MultiplyModule::Initialise()
{
	for (int i = 0; i < _inputs.GetSize(); ++i)
		if (_inputs[i].IsConnected())
			_activeInputs.Push(&_inputs[i]);
}

void MultiplyModule::Update()
{
	const int count = _activeInputs.GetSize();
	if (count == 0)
		return;

	float total = 1;
	for (int i = 0; i < count; ++i)
		total *= _activeInputs[i]->GetValue();

	_outputs[Pin::Mixer::Output::Signal].SetValue(total);
}
