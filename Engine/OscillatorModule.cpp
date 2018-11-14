#include "OscillatorModule.h"
#include "Util.h"

using namespace Engine;

OscillatorModule::OscillatorModule()
{
	_multiInputs.SetSize(Pin::Oscillator::Input::_Count);
	_multiOutputs.SetSize(Pin::Oscillator::Output::_Count);
}

void OscillatorModule::Update()
{
	if (_instances.GetSize() == 0)
		_instances.SetSize(_multiInputs[0].GetSize());

	for (int i = 0; i < _multiInputs[0].GetSize(); ++i)
		UpdateInstance(i);
}

void OscillatorModule::UpdateInstance(int index)
{
	Input& levelInput = _multiInputs[Pin::Oscillator::Input::Level][index];
	Input& pitchInput = _multiInputs[Pin::Oscillator::Input::Pitch][index];
	Output& signalOutput = _multiOutputs[Pin::Oscillator::Output::Signal][index];

	const float level = levelInput.GetValue();

	Instance& instance = _instances[index];

	if (level == 0)
	{
		signalOutput.SetValue(0);
		instance._phase = 0;
		return;
	}

	if (pitchInput.HasChanged())
	{
		pitchInput.ResetChanged();
		instance._phaseDelta = ::PitchToPhaseDelta(pitchInput.GetValue());
	}

	instance._phase += instance._phaseDelta;
	
	uint16_t phase = instance._phase + static_cast<int16_t>(_multiInputs[Pin::Oscillator::Input::PhaseMod][index].GetValue() * 0x7fff);
	const byte waveform = static_cast<byte>(_multiInputs[Pin::Oscillator::Input::Waveform][index].GetValue());
	uint16_t output = ::SampleWaveform(waveform, phase, 0x8000);

	signalOutput.SetValue((output * Config::uint16ToFloat - 0.5f) * level);
}
