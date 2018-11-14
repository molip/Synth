#include "DelayModule.h"
#include "Util.h"

using namespace Engine;

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

		_clear = newPeriod > _period;
		_period = newPeriod;
	}

	float val = signalInput.GetValue();
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
