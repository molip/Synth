#include "FilterModule.h"

using namespace Engine;

FilterModule::FilterModule()
{
	_inputs.SetSize(Pin::Filter::Input::_Count);
	_outputs.SetSize(Pin::Filter::Output::_Count);
}

void FilterModule::Update()
{
	Input& signalInput = _inputs[Pin::Filter::Input::Signal];
	Output& signalOutput = _outputs[Pin::Filter::Output::Signal];

	if (_inputs[Pin::Filter::Input::Bypass].GetValue())
	{
		signalOutput.SetValue(signalInput.GetValue());
		return;
	}

	Input& freqInput = _inputs[Pin::Filter::Input::Frequency];
	Input& resInput = _inputs[Pin::Filter::Input::Resonance];

	if (freqInput.HasChanged() || resInput.HasChanged())
	{
		freqInput.ResetChanged();
		resInput.ResetChanged();
		float freq = freqInput.GetValue() * 0.25f; // Arbitrary scale. 
		float res = resInput.GetValue();
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
