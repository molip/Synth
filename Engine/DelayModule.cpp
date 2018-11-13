#include "DelayModule.h"
#include "Util.h"

using namespace Engine;

DelayModule::DelayModule()
{
	_unsignedInputs.SetSize(Pin::Delay::UnsignedInput::_Count);
	_signedInputs.SetSize(Pin::Delay::SignedInput::_Count);
	_signedOutputs.SetSize(Pin::Delay::SignedOutput::_Count);

	_buffer.SetSize(Config::sampleRate); // 1 second.
}

void DelayModule::Update()
{
	SignedInput& signalInput = _signedInputs[Pin::Delay::SignedInput::Signal];
	SignedOutput& signalOutput = _signedOutputs[Pin::Delay::SignedOutput::Signal];
	SignedInput& feedbackInput = _signedInputs[Pin::Delay::SignedInput::Feedback];
	UnsignedInput& periodInput = _unsignedInputs[Pin::Delay::UnsignedInput::Period];

	if (feedbackInput.HasChanged())
	{
		feedbackInput.ResetChanged();
		_feedback = feedbackInput.GetValue();
	}

	if (periodInput.HasChanged())
	{
		periodInput.ResetChanged();

		unsigned_t newPeriod = periodInput.GetValue();
		if (newPeriod > 1000)
			newPeriod = 1000;
		
		newPeriod *= Config::sampleRateMS;

		_clear = newPeriod > _period;
		_period = newPeriod;
	}

	signed_t val = signalInput.GetValue();
	if (!_clear)
		val = ClipBipolar(val + _buffer[_current]);

	_buffer[_current++] = val * _feedback;

	if (_current >= _period)
	{
		_clear = false;
		_current = 0;
	}

	signalOutput.SetValue(val);
}
