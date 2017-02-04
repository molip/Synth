#include "OutputModule.h"

OutputModule::OutputModule()
{
	_signedInputs.SetSize(Pin::Target::SignedInput::_Count);
}

void OutputModule::Update()
{
	const SignedInput& signal = _signedInputs[Pin::Target::SignedInput::Signal];
	analogWrite(A21, (signal.GetValue() + 0x8000) >> 4);
}
