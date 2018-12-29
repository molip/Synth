#include "NoiseModule.h"
#include "Util.h"

using namespace Engine;

NoiseModule::NoiseModule()
{
	_inputs.SetSize(Pin::Noise::Input::_Count);
	_outputs.SetSize(Pin::Noise::Output::_Count);
}

void NoiseModule::Update()
{
	Input& pitchInput = _inputs[Pin::Noise::Input::Pitch];
	Input& levelInput = _inputs[Pin::Noise::Input::Level];
	Output& signalOutput = _outputs[Pin::Noise::Output::Signal];

	const float level = levelInput.GetValue();

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

	if (_phase & 0xffff0000)
	{
		_phase &= 0xffff;
		uint16_t val = ::rand();
		signalOutput.SetValue((val * Config::uint16ToFloat2 - 1.0f) * level);
	}
}
