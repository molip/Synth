#include "RangeModule.h"

using namespace Engine;

RangeModule::RangeModule()
{
	_unsignedInputs.SetSize(Pin::Range::UnsignedInput::_Count);
	_unsignedOutputs.SetSize(Pin::Range::UnsignedOutput::_Count);
}

void RangeModule::Update()
{
	unsigned_t signal = _unsignedInputs[Pin::Range::UnsignedInput::Value].GetValue();
	unsigned_t min = _unsignedInputs[Pin::Range::UnsignedInput::Minimum].GetValue();
	unsigned_t max = _unsignedInputs[Pin::Range::UnsignedInput::Maximum].GetValue();

	unsigned_t output = unsigned_t(min + uint32_t(max - min) * signal / 0xffff); // Rounding down.
	_unsignedOutputs[Pin::Range::UnsignedOutput::Value].SetValue(output);
}
