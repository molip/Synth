#include "Module.h"

using namespace Engine;

void Module::SetPolyphony(int polyphony)
{
	for (int i = 0; i < _signedMultiOutputs.GetSize(); ++i)
		_signedMultiOutputs[i].SetSize(polyphony);
	
	for (int i = 0; i < _signedMultiInputs.GetSize(); ++i)
		_signedMultiInputs[i].SetSize(polyphony);	
}

namespace Engine
{
template<> Array<SignedOutput>& Module::GetPins() { return _signedOutputs; }
template<> Array<SignedInput>& Module::GetPins() { return _signedInputs; }

template<> Array<SignedMultiOutput>& Module::GetMultiPins() { return _signedMultiOutputs; }
template<> Array<SignedMultiInput>& Module::GetMultiPins() { return _signedMultiInputs; }
}