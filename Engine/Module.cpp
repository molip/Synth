#include "Module.h"

using namespace Engine;

void Module::SetPolyphony(int polyphony)
{
	for (int i = 0; i < _signedMultiOutputs.GetSize(); ++i)
		_signedMultiOutputs[i].SetSize(polyphony);
	
	for (int i = 0; i < _unsignedMultiOutputs.GetSize(); ++i)
		_unsignedMultiOutputs[i].SetSize(polyphony);

	for (int i = 0; i < _signedMultiInputs.GetSize(); ++i)
		_signedMultiInputs[i].SetSize(polyphony);
	
	for (int i = 0; i < _unsignedMultiInputs.GetSize(); ++i)
		_unsignedMultiInputs[i].SetSize(polyphony);
}

namespace Engine
{
template<> Array<SignedOutput>& Module::GetPins() { return _signedOutputs; }
template<> Array<UnsignedOutput>& Module::GetPins() { return _unsignedOutputs; }
template<> Array<SignedInput>& Module::GetPins() { return _signedInputs; }
template<> Array<UnsignedInput>& Module::GetPins() { return _unsignedInputs; }

template<> Array<SignedMultiOutput>& Module::GetMultiPins() { return _signedMultiOutputs; }
template<> Array<UnsignedMultiOutput>& Module::GetMultiPins() { return _unsignedMultiOutputs; }
template<> Array<SignedMultiInput>& Module::GetMultiPins() { return _signedMultiInputs; }
template<> Array<UnsignedMultiInput>& Module::GetMultiPins() { return _unsignedMultiInputs; }
}