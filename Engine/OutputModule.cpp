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
	_result = ClipBipolar(signal.GetValue());
}
