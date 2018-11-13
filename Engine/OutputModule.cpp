#include "OutputModule.h"
#include "Util.h"

using namespace Engine;

OutputModule::OutputModule()
{
	_signedInputs.SetSize(Pin::Target::SignedInput::_Count);
}

void OutputModule::Update()
{
	const SignedInput& signal = _signedInputs[Pin::Target::SignedInput::Signal];
	OUTPUT_AUDIO(0x800 + static_cast<int16_t>(ClipBipolar(signal.GetValue()) * 0xffe)); // 12 bit.
}
