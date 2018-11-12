#include "RangeModule.h"

using namespace Engine;

RangeModule::RangeModule()
{
	_signedInputs.SetSize(Pin::Range::SignedInput::_Count);
	_signedOutputs.SetSize(Pin::Range::SignedOutput::_Count);
}

void RangeModule::Update()
{
	signed_t signal = _signedInputs[Pin::Range::SignedInput::Value].GetValue();
	signed_t min = _signedInputs[Pin::Range::SignedInput::Minimum].GetValue();
	signed_t max = _signedInputs[Pin::Range::SignedInput::Maximum].GetValue();

	signed_t output = min + (max - min) * signal;
	_signedOutputs[Pin::Range::SignedOutput::Value].SetValue(output);
}
