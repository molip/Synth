#include "FilterModule.h"

FilterModule::FilterModule()
{
	_unsignedInputs.SetSize(Pin::Filter::UnsignedInput::_Count);
	_signedInputs.SetSize(Pin::Filter::SignedInput::_Count);
	_signedOutputs.SetSize(Pin::Filter::SignedOutput::_Count);
}

void FilterModule::Update()
{
	SignedInput& signalInput = _signedInputs[Pin::Filter::SignedInput::Signal];
	SignedOutput& signalOutput = _signedOutputs[Pin::Filter::SignedOutput::Signal];

	if (_unsignedInputs[Pin::Filter::UnsignedInput::Bypass].GetValue())
	{
		signalOutput.SetValue(signalInput.GetValue());
		return;
	}

	UnsignedInput& freqInput = _unsignedInputs[Pin::Filter::UnsignedInput::Frequency];
	UnsignedInput& resInput = _unsignedInputs[Pin::Filter::UnsignedInput::Resonance];

	if (freqInput.HasChanged() || resInput.HasChanged())
	{
		freqInput.ResetChanged();
		resInput.ResetChanged();
		float freq = freqInput.GetValue() * Config::divUnsignedMax * 0.25f; // Arbitrary scale. 
		float res = resInput.GetValue() * Config::divUnsignedMax;
		_q = 1.0f - freq;
		_p = freq + 0.8f * freq * _q;
		_f = _p + _p - 1.0f;
		_q = res * (1.0f + 0.5f * _q * (1.0f - _q + 5.6f * _q * _q));
	}

	float in = signalInput.GetValue();

	in -= _q * _b4;                          //feedback
	float t1 = _b1;
	_b1 = (in + _b0) * _p - _b1 * _f;
	float t2 = _b2;
	_b2 = (_b1 + t1) * _p - _b2 * _f;
	t1 = _b3;
	_b3 = (_b2 + t2) * _p - _b3 * _f;
	_b4 = (_b3 + t1) * _p - _b4 * _f;
	_b4 = _b4 - _b4 * _b4 * _b4 * 0.166667f;    //clipping
	_b0 = in;

	signalOutput.SetValue(_b4); // Signed.
}
