#include "MathsModule.h"

using namespace Engine;

MathsModule::MathsModule()
{
	_inputs.SetSize(Pin::Maths::Input::_Count);
	_outputs.SetSize(Pin::Maths::Output::_Count);
}

void MathsModule::Update()
{
	float signal = _inputs[Pin::Maths::Input::Value].GetValue();
	float add = _inputs[Pin::Maths::Input::Add].GetValue();
	float mult = _inputs[Pin::Maths::Input::Multiply].GetValue();

	float output = add + signal * mult;
	_outputs[Pin::Maths::Output::Value].SetValue(output);
}
