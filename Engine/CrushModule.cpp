#include "CrushModule.h"
#include "Util.h"

using namespace Engine;

CrushModule::CrushModule()
{
	_inputs.SetSize(Pin::Crush::Input::_Count);
	_outputs.SetSize(Pin::Crush::Output::_Count);
}

void CrushModule::Update()
{
	Input& signalInput = _inputs[Pin::Crush::Input::Signal];
	Input& frequencyInput = _inputs[Pin::Crush::Input::Frequency];
	Input& amplitudeInput = _inputs[Pin::Crush::Input::Amplitude];
	Output& signalOutput = _outputs[Pin::Crush::Output::Signal];

	if (frequencyInput.HasChanged())
	{
		frequencyInput.ResetChanged();
		const float freq = ClipUnipolar(frequencyInput.GetValue());
		_phaseDelta = freq < 1 ? 0.02f + freq * freq* 0.3f : 1;
	}

	_phase += _phaseDelta;

	if (_phase >= 1)
	{
		_phase -= 1;
		_signal = signalInput.GetValue();

		if (amplitudeInput.HasChanged())
		{
			amplitudeInput.ResetChanged();
			const float amplitude = ClipUnipolar(amplitudeInput.GetValue());
			_quantum = amplitude < 1 ? 1 - (0.3f + amplitude * 0.7f) : 0;
			_quantumInv = amplitude < 1 ? 1 / _quantum : 0;
		}

		if (_quantum > 0)
		{
			const int16_t level = int16_t(0.5f + _signal * _quantumInv);
			_signal = float(level * _quantum);
		}
	}

	signalOutput.SetValue(_signal);
}
