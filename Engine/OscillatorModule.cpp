#include "OscillatorModule.h"
#include "Util.h"

using namespace Engine;

OscillatorModule::OscillatorModule()
{
	_inputs.SetSize(Pin::Oscillator::Input::_Count);
	_outputs.SetSize(Pin::Oscillator::Output::_Count);
}

void OscillatorModule::Update()
{
	Input& levelInput = _inputs[Pin::Oscillator::Input::Level];
	Input& pitchInput = _inputs[Pin::Oscillator::Input::Pitch];
	Input& phaseModInput = _inputs[Pin::Oscillator::Input::PhaseMod];
	Output& signalOutput = _outputs[Pin::Oscillator::Output::Signal];

	const float maxLevelDelta = 0.1f; // 1ms 1.0 release at 40kHz: delta = 0.025 / sample. 

	const float thisLevel = levelInput.GetValue();
	
	float level;
	if (_hold) // Continue hold.
	{
		level = _lastLevel;
	}
	else if (_lastLevel - thisLevel > maxLevelDelta) // Start hold.
	{
		_hold = true;
		level = _lastLevel;
	}
	else
	{
		if (thisLevel == 0)
		{
			signalOutput.SetValue(0);
			_lastLevel = 0;
			_phase = 0;
			return;
		}

		level = thisLevel;
	}

	if (pitchInput.HasChanged())
	{
		pitchInput.ResetChanged();
		_phaseDelta = ::PitchToPhaseDelta(pitchInput.GetValue());
	}

	const int16_t phaseMod = phaseModInput.IsConnected() ? static_cast<int16_t>(phaseModInput.GetValue() * 0x7fff) : 0;

	_phase += _phaseDelta;
	
	uint16_t phase = _phase + phaseMod;
	const byte waveform = static_cast<byte>(_inputs[Pin::Oscillator::Input::Waveform].GetValue());
	uint16_t output = ::SampleWaveform(waveform, phase, 0x8000, &_ctx);

	const bool positive = output & 0x8000;
	if (_hold && positive != _positive) // Crossed zero, good time to cancel hold and change level. 
	{
		_hold = false;
		level = thisLevel;
	}

	signalOutput.SetValue((output * Config::uint16ToFloat2 - 1.0f) * level);
	_positive = positive;
	_lastLevel = level;
}
