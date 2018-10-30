#include "OutputModule.h"
#include "Util.h"

OutputModule::OutputModule()
{
	_signedInputs.SetSize(Pin::Target::SignedInput::_Count);
}

void OutputModule::Update()
{
	const SignedInput& signal = _signedInputs[Pin::Target::SignedInput::Signal];
	OUTPUT_AUDIO(0x800 + ClipSigned(signal.GetValue()) * 0x7ff); // 12 bit.
}
