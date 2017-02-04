#include "Module.h"

void Module::SetPolyphony(int polyphony)
{
	for (int i = 0; i < _signedPolyOutputs.GetSize(); ++i)
		_signedPolyOutputs[i].SetSize(polyphony);
	
	for (int i = 0; i < _unsignedPolyOutputs.GetSize(); ++i)
		_unsignedPolyOutputs[i].SetSize(polyphony);

	for (int i = 0; i < _signedPolyInputs.GetSize(); ++i)
		_signedPolyInputs[i].SetSize(polyphony);
	
	for (int i = 0; i < _unsignedPolyInputs.GetSize(); ++i)
		_unsignedPolyInputs[i].SetSize(polyphony);
}

template<> Array<SignedOutput>& Module::GetPins() { return _signedOutputs; }
template<> Array<UnsignedOutput>& Module::GetPins() { return _unsignedOutputs; }
template<> Array<SignedInput>& Module::GetPins() { return _signedInputs; }
template<> Array<UnsignedInput>& Module::GetPins() { return _unsignedInputs; }

template<> Array<SignedPolyOutput>& Module::GetPolyPins() { return _signedPolyOutputs; }
template<> Array<UnsignedPolyOutput>& Module::GetPolyPins() { return _unsignedPolyOutputs; }
template<> Array<SignedPolyInput>& Module::GetPolyPins() { return _signedPolyInputs; }
template<> Array<UnsignedPolyInput>& Module::GetPolyPins() { return _unsignedPolyInputs; }



UnsignedValueModule::UnsignedValueModule()
{
	_unsignedOutputs.SetSize(1);
}

void UnsignedValueModule::SetValue(unsigned_t val)
{
	_unsignedOutputs[0].SetValue(val);
}
