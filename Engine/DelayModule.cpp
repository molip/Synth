#include "DelayModule.h"
#include "Util.h"

using namespace Engine;

Array<float> DelayModule::_buffer;

DelayModule::DelayModule()
{
	_inputs.SetSize(Pin::Delay::Input::_Count);
	_outputs.SetSize(Pin::Delay::Output::_Count);

	_buffer.SetSize(Config::sampleRate); // 1 second.
}

void DelayModule::Update()
{
	Input& signalInput = _inputs[Pin::Delay::Input::Signal];
	Output& signalOutput = _outputs[Pin::Delay::Output::Signal];
	Input& feedbackInput = _inputs[Pin::Delay::Input::Feedback];
	Input& periodInput = _inputs[Pin::Delay::Input::Period];
	Input& wetInput = _inputs[Pin::Delay::Input::Wet];
	Input& dryInput = _inputs[Pin::Delay::Input::Dry];

	if (feedbackInput.HasChanged())
	{
		feedbackInput.ResetChanged();
		_feedback = feedbackInput.GetValue();
	}

	if (periodInput.HasChanged())
	{
		periodInput.ResetChanged();

		uint16_t newPeriod = static_cast<uint16_t>(periodInput.GetValue());
		if (newPeriod > 1000)
			newPeriod = 1000;
		
		newPeriod *= Config::sampleRateMS;

		_clear = newPeriod > _period; // Ignore invalid data from here to end of buffer.
		_period = newPeriod;
	}

	const float wet = wetInput.GetValue();
	const float dry = dryInput.GetValue();
	const float val = signalInput.GetValue();
	const float oldVal = _clear ? 0 : _buffer[_current];

	// Feedback = 0 should produce a single, full volume echo.
	_buffer[_current++] = val + oldVal * _feedback;

	if (_current >= _period)
	{
		_clear = false;
		_current = 0;
	}

	signalOutput.SetValue(val * dry + oldVal * wet);
}
