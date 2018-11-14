#include "RangeModule.h"

using namespace Engine;

RangeModule::RangeModule()
{
	_inputs.SetSize(Pin::Range::Input::_Count);
	_outputs.SetSize(Pin::Range::Output::_Count);
}

void RangeModule::Update()
{
	float signal = _inputs[Pin::Range::Input::Value].GetValue();
	float min = _inputs[Pin::Range::Input::Minimum].GetValue();
	float max = _inputs[Pin::Range::Input::Maximum].GetValue();

	float output = min + (max - min) * signal;
	_outputs[Pin::Range::Output::Value].SetValue(output);
}
