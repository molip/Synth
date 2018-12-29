#include "LFOModule.h"
#include "Util.h"

using namespace Engine;

LFOModule::LFOModule()
{
	_inputs.SetSize(Pin::LFO::Input::_Count);
	_outputs.SetSize(Pin::LFO::Output::_Count);
}

void LFOModule::Update()
{
	Input& triggerInput = _inputs[Pin::LFO::Input::Trigger];
	if (triggerInput.HasChanged())
	{
		if (triggerInput.GetValue())
			_phase = 0;
		triggerInput.ResetChanged();
	}

	if (++_divide < 256) // [1/32 Hz, 48 Hz]
		return;

	_divide = 0;

	Input& waveformInput = _inputs[Pin::LFO::Input::Waveform];
	Input& levelInput = _inputs[Pin::LFO::Input::Level];
	Input& pitchInput = _inputs[Pin::LFO::Input::Pitch];
	Input& dutyInput = _inputs[Pin::LFO::Input::Duty];
	Output& signalOutput = _outputs[Pin::LFO::Output::Signal];

	const float level = levelInput.GetValue();

	if (level == 0)
	{
		signalOutput.SetValue(0);
		_phase = 0;
		return;
	}

	if (dutyInput.HasChanged())
	{
		dutyInput.ResetChanged();
		_dutyFixed = FloatToFixed16(dutyInput.GetValue());
	}

	if (pitchInput.HasChanged())
	{
		pitchInput.ResetChanged();
		_phaseDelta = ::PitchToPhaseDelta(pitchInput.GetValue());
	}

	const byte waveform = static_cast<byte>(waveformInput.GetValue());
	if (waveformInput.HasChanged())
	{
		waveformInput.ResetChanged();
		_phaseAdjust = waveform == 0 ? 0x8000 : waveform == 2 ? 0 : 0xc000;
	}

	_phase += _phaseDelta;
	
	const uint16_t output = ::SampleWaveform(waveform, _phase + _phaseAdjust, _dutyFixed, nullptr);

	signalOutput.SetValue(output * Config::uint16ToFloat * level);
}
