#include "RangeModule.h"

RangeModule::RangeModule()
{
	_unsignedInputs.SetSize(Pin::Range::UnsignedInput::_Count);
	_unsignedOutputs.SetSize(Pin::Range::UnsignedOutput::_Count);
}

void RangeModule::Update()
{
	uint16_t signal = _unsignedInputs[Pin::Range::UnsignedInput::Value].GetValue();
	uint16_t min = _unsignedInputs[Pin::Range::UnsignedInput::Minimum].GetValue();
	uint16_t max = _unsignedInputs[Pin::Range::UnsignedInput::Maximum].GetValue();

	uint16_t output = uint16_t(min + uint32_t(max - min) * signal / 0xffff); // Rounding down.
	_unsignedOutputs[Pin::Range::UnsignedOutput::Value].SetValue(output);
}
