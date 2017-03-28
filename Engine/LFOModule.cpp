#include "LFOModule.h"
#include "Util.h"

LFOModule::LFOModule()
{
	_unsignedInputs.SetSize(Pin::LFO::UnsignedInput::_Count);
	_unsignedOutputs.SetSize(Pin::LFO::UnsignedOutput::_Count);
}

void LFOModule::Update()
{
	UnsignedInput& triggerInput = _unsignedInputs[Pin::LFO::UnsignedInput::Trigger];
	if (triggerInput.HasChanged())
	{
		if (triggerInput.GetValue())
			_phase = 0;
		triggerInput.ResetChanged();
	}

	if (++_divide < 256) // [1/32 Hz, 48 Hz]
		return;

	_divide = 0;

	UnsignedInput& waveformInput = _unsignedInputs[Pin::LFO::UnsignedInput::Waveform];
	UnsignedInput& levelInput = _unsignedInputs[Pin::LFO::UnsignedInput::Level];
	UnsignedInput& pitchInput = _unsignedInputs[Pin::LFO::UnsignedInput::Pitch];
	UnsignedInput& dutyInput = _unsignedInputs[Pin::LFO::UnsignedInput::Duty];
	UnsignedOutput& signalOutput = _unsignedOutputs[Pin::LFO::UnsignedOutput::Signal];

	unsigned_t level = levelInput.GetValue();
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

	_phase += _phaseDelta;
	
	const uint32_t output = ::SampleWaveform(waveformInput.GetValue(), _phase, dutyInput.GetValue());

	signalOutput.SetValue((output * level) >> 16);
}
