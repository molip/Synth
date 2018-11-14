#include "LFOModule.h"
#include "Util.h"

using namespace Engine;

LFOModule::LFOModule()
{
	_signedInputs.SetSize(Pin::LFO::SignedInput::_Count);
	_signedOutputs.SetSize(Pin::LFO::SignedOutput::_Count);
}

void LFOModule::Update()
{
	SignedInput& triggerInput = _signedInputs[Pin::LFO::SignedInput::Trigger];
	if (triggerInput.HasChanged())
	{
		if (triggerInput.GetValue())
			_phase = 0;
		triggerInput.ResetChanged();
	}

	if (++_divide < 256) // [1/32 Hz, 48 Hz]
		return;

	_divide = 0;

	SignedInput& waveformInput = _signedInputs[Pin::LFO::SignedInput::Waveform];
	SignedInput& levelInput = _signedInputs[Pin::LFO::SignedInput::Level];
	SignedInput& pitchInput = _signedInputs[Pin::LFO::SignedInput::Pitch];
	SignedInput& dutyInput = _signedInputs[Pin::LFO::SignedInput::Duty];
	SignedOutput& signalOutput = _signedOutputs[Pin::LFO::SignedOutput::Signal];

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
	
	const uint16_t output = ::SampleWaveform(waveform, _phase + _phaseAdjust, _dutyFixed);

	signalOutput.SetValue(output * Config::uint16ToFloat * level);
}
