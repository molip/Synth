#include "KnobModule.h"
#include "Config.h"

using namespace Engine;

KnobModule::KnobModule() 
{
	_inputs.SetSize(KnobCount);
	_outputs.SetSize(KnobCount);
	_activeInputs.Reserve(KnobCount);
	_activeOutputs.Reserve(KnobCount);
	_values.Reserve(KnobCount);
}

void KnobModule::Initialise()
{
	for (int i = 0; i < KnobCount; ++i)
	{
		if (_outputs[i].IsConnected())
		{
			_activeInputs.Push(&_inputs[i]);
			_activeOutputs.Push(&_outputs[i]);
			_values.Push(0);
		}
	}

	for (int i = 0; i < _activeInputs.GetSize(); ++i)
		UpdateValue(i);
}

void KnobModule::UpdateValue(int iActive)
{
	_activeOutputs[iActive]->SetValue(_values[iActive] * Config::analogInToFloat);
}

void KnobModule::UpdateRemote()
{
	for (int i = 0; i < _activeInputs.GetSize(); ++i)
	{
		auto* remote = _activeInputs[i];
		if (remote->HasChanged())
		{
			remote->ResetChanged();
			_activeOutputs[i]->SetValue(remote->GetValue());
		}
	}
}

void KnobModule::SetValue(int iActive, uint16_t val)
{
	if (_values[iActive] != val)
	{
		_values[iActive] = val;
		UpdateValue(iActive);
	}
}
