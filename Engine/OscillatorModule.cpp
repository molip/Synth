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

	float level = levelInput.GetValue();
	float delta = level - _lastLevel;
	const float minDecTime = 10.0f; // ms for 1 -> 0 level change.
	const float maxDec = 1 / (minDecTime * 40);
	
	if (delta < -maxDec)
		level = _lastLevel - maxDec;

	_lastLevel = level;

	if (level == 0)
	{
		signalOutput.SetValue(0);
		_phase = 0;
		return;
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

	signalOutput.SetValue((output * Config::uint16ToFloat2 - 1.0f) * level);
}
