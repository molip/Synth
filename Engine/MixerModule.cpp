#include "MixerModule.h"

MixerModule::MixerModule(int inputCount) : _inputCount(inputCount)
{
	_inputs = new SignedInput[inputCount];
}

MixerModule::~MixerModule()
{
	delete [] _inputs;
}

void MixerModule::Update()
{
	uint32_t total = 0;
	for (int i = 0; i < _inputCount; ++i)
		total += _inputs[i].GetValue();

	_ouput.SetValue(total / _inputCount);
}
