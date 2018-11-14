#include "Module.h"

using namespace Engine;

void Module::SetPolyphony(int polyphony)
{
	// TODO: poly inputs/outputs

	for (int i = 0; i < _multiOutputs.GetSize(); ++i)
		_multiOutputs[i].SetSize(polyphony);
	
	for (int i = 0; i < _multiInputs.GetSize(); ++i)
		_multiInputs[i].SetSize(polyphony);	
}

namespace Engine
{
template<> Array<Output>& Module::GetPins() { return _outputs; }
template<> Array<Input>& Module::GetPins() { return _inputs; }

template<> Array<MultiOutput>& Module::GetMultiPins() { return _multiOutputs; }
template<> Array<MultiInput>& Module::GetMultiPins() { return _multiInputs; }
}