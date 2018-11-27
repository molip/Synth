#include "OutputModule.h"
#include "Util.h"

using namespace Engine;

OutputModule::OutputModule()
{
	_inputs.SetSize(Pin::Target::Input::_Count);
}

void OutputModule::Update()
{
	const Input& signal = _inputs[Pin::Target::Input::Signal];
	OUTPUT_AUDIO(0x800 + static_cast<int16_t>(ClipBipolar(signal.GetValue()) * 0x7ff)); // 12 bit.
}
