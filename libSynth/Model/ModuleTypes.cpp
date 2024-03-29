#include "stdafx.h"
#include "ModuleTypes.h"

#include "../../libKernel/Debug.h"
#include "../../libKernel/FormatString.h"

using namespace Synth::Model;

const ModuleTypes& ModuleTypes::Instance()
{
	static ModuleTypes instance;
	return instance;
}

ModuleTypes::ModuleTypes()
{
	_types.emplace_back("midi", "MIDI", Engine::ModuleType::MIDI);
	_types.back().AddOutput(std::make_unique<PinType>("ptch", "Pitch", PinType::ConnectionType::Multi, Engine::Pin::MIDI::MultiOutput::Pitch));
	_types.back().AddOutput(std::make_unique<PinType>("gate", "Gate", PinType::ConnectionType::Multi, Engine::Pin::MIDI::MultiOutput::Gate));

	_types.emplace_back("envl", "Envelope", Engine::ModuleType::Envelope);
	_types.back().AddInput(std::make_unique<PinType>("gate", "Gate", PinType::ConnectionType::Single, Engine::Pin::Envelope::Input::Gate));
	_types.back().AddInput(std::make_unique<PinType>("atck", "Attack", PinType::ConnectionType::Single, Engine::Pin::Envelope::Input::Attack)).SetValueType(std::make_unique<TimeValueType>(50));
	_types.back().AddInput(std::make_unique<PinType>("hold", "Hold", PinType::ConnectionType::Single, Engine::Pin::Envelope::Input::Hold)).SetValueType(std::make_unique<TimeValueType>(50));
	_types.back().AddInput(std::make_unique<PinType>("decy", "Decay", PinType::ConnectionType::Single, Engine::Pin::Envelope::Input::Decay)).SetValueType(std::make_unique<TimeValueType>(50));
	_types.back().AddInput(std::make_unique<PinType>("sust", "Sustain", PinType::ConnectionType::Single, Engine::Pin::Envelope::Input::Sustain)).SetValueType(std::make_unique<PercentValueType>(50, 50, 0, 100));
	_types.back().AddInput(std::make_unique<PinType>("rels", "Release", PinType::ConnectionType::Single, Engine::Pin::Envelope::Input::Release)).SetValueType(std::make_unique<TimeValueType>(400));
	_types.back().AddInput(std::make_unique<PinType>("latc", "Latch", PinType::ConnectionType::Single, Engine::Pin::Envelope::Input::Latch)).SetValueType(std::make_unique<BoolValueType>(0));
	_types.back().AddOutput(std::make_unique<PinType>("levl", "Level", PinType::ConnectionType::Single, Engine::Pin::Envelope::Output::Level)).SetMonitor();
		
	_types.emplace_back("oscl", "Oscillator", Engine::ModuleType::Oscillator);
	_types.back().AddInput(std::make_unique<PinType>("wave", "Waveform", PinType::ConnectionType::Single, Engine::Pin::Oscillator::Input::Waveform)).SetValueType(std::make_unique<IntValueType>(0, 1, 0, 5));
	_types.back().AddInput(std::make_unique<PinType>("ptch", "Pitch", PinType::ConnectionType::Single, Engine::Pin::Oscillator::Input::Pitch)).SetValueType(std::make_unique<PitchValueType>());
	_types.back().AddInput(std::make_unique<PinType>("levl", "Level", PinType::ConnectionType::Single, Engine::Pin::Oscillator::Input::Level)).SetValueType(std::make_unique<PercentValueType>(0, 100, 0, 1000));
	_types.back().AddInput(std::make_unique<PinType>("pmod", "PhaseMod", PinType::ConnectionType::Single, Engine::Pin::Oscillator::Input::PhaseMod)).SetValueType(std::make_unique<PercentValueType>(0, 100, -100, 100));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, Engine::Pin::Oscillator::Output::Signal));

	_types.emplace_back("pmix", "PolyMixer", Engine::ModuleType::PolyMixer);
	_types.back().AddInput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Multi, Engine::Pin::PolyMixer::MultiInput::Signal));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, Engine::Pin::PolyMixer::Output::Signal));

	_types.emplace_back("trgt", "Target", Engine::ModuleType::Target);
	_types.back().AddInput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, Engine::Pin::Target::Input::Signal)).SetValueType(std::make_unique<PercentValueType>(0, 100, -100, 100));;

	_types.emplace_back("filt", "Filter", Engine::ModuleType::Filter);
	_types.back().AddInput(std::make_unique<PinType>("freq", "Frequency", PinType::ConnectionType::Single, Engine::Pin::Filter::Input::Frequency)).SetValueType(std::make_unique<PercentValueType>(0, 100, 0, 100));
	_types.back().AddInput(std::make_unique<PinType>("resn", "Resonance", PinType::ConnectionType::Single, Engine::Pin::Filter::Input::Resonance)).SetValueType(std::make_unique<PercentValueType>(0, 100, 0, 100));
	_types.back().AddInput(std::make_unique<PinType>("byps", "Bypass", PinType::ConnectionType::Single, Engine::Pin::Filter::Input::Bypass)).SetValueType(std::make_unique<BoolValueType>(0));
	_types.back().AddInput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, Engine::Pin::Filter::Input::Signal)).SetValueType(std::make_unique<PercentValueType>(0, 100, -100, 100));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, Engine::Pin::Filter::Output::Signal));

	_types.emplace_back("math", "Maths", Engine::ModuleType::Maths);
	_types.back().AddInput(std::make_unique<PinType>("valu", "Value", PinType::ConnectionType::Single, Engine::Pin::Maths::Input::Value)).SetValueType(std::make_unique<PercentValueType>(0, 100, -100000000, 100000000));
	_types.back().AddInput(std::make_unique<PinType>("add_", "Add", PinType::ConnectionType::Single, Engine::Pin::Maths::Input::Add)).SetValueType(std::make_unique<PercentValueType>(0, 100, -100000000, 100000000));
	_types.back().AddInput(std::make_unique<PinType>("mult", "Multiply", PinType::ConnectionType::Single, Engine::Pin::Maths::Input::Multiply)).SetValueType(std::make_unique<PercentValueType>(0, 100, -100000000, 100000000));
	_types.back().AddOutput(std::make_unique<PinType>("valu", "Value", PinType::ConnectionType::Single, Engine::Pin::Maths::Output::Value));

	_types.emplace_back("mixr", "Mixer", Engine::ModuleType::Mixer);
	_types.back().AddInput(std::make_unique<PinType>("sig0", "Signal 0", PinType::ConnectionType::Single, Engine::Pin::Mixer::Input::Signal0)).SetValueType(std::make_unique<PercentValueType>(0, 100, -100, 100));
	_types.back().AddInput(std::make_unique<PinType>("sig1", "Signal 1", PinType::ConnectionType::Single, Engine::Pin::Mixer::Input::Signal1)).SetValueType(std::make_unique<PercentValueType>(0, 100, -100, 100));
	_types.back().AddInput(std::make_unique<PinType>("sig2", "Signal 2", PinType::ConnectionType::Single, Engine::Pin::Mixer::Input::Signal2)).SetValueType(std::make_unique<PercentValueType>(0, 100, -100, 100));
	_types.back().AddInput(std::make_unique<PinType>("sig3", "Signal 3", PinType::ConnectionType::Single, Engine::Pin::Mixer::Input::Signal3)).SetValueType(std::make_unique<PercentValueType>(0, 100, -100, 100));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, Engine::Pin::Mixer::Output::Signal));

	_types.emplace_back("mult", "Multiply", Engine::ModuleType::Multiply);
	_types.back().AddInput(std::make_unique<PinType>("sig0", "Signal 0", PinType::ConnectionType::Single, Engine::Pin::Multiply::Input::Signal0)).SetValueType(std::make_unique<PercentValueType>(0, 100, -100, 100));
	_types.back().AddInput(std::make_unique<PinType>("sig1", "Signal 1", PinType::ConnectionType::Single, Engine::Pin::Multiply::Input::Signal1)).SetValueType(std::make_unique<PercentValueType>(0, 100, -100, 100));
	_types.back().AddInput(std::make_unique<PinType>("sig2", "Signal 2", PinType::ConnectionType::Single, Engine::Pin::Multiply::Input::Signal2)).SetValueType(std::make_unique<PercentValueType>(0, 100, -100, 100));
	_types.back().AddInput(std::make_unique<PinType>("sig3", "Signal 3", PinType::ConnectionType::Single, Engine::Pin::Multiply::Input::Signal3)).SetValueType(std::make_unique<PercentValueType>(0, 100, -100, 100));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, Engine::Pin::Multiply::Output::Signal));

	_types.emplace_back("lfo", "LFO", Engine::ModuleType::LFO);
	_types.back().AddInput(std::make_unique<PinType>("trig", "Trigger", PinType::ConnectionType::Single, Engine::Pin::LFO::Input::Trigger));
	_types.back().AddInput(std::make_unique<PinType>("wave", "Waveform", PinType::ConnectionType::Single, Engine::Pin::LFO::Input::Waveform)).SetValueType(std::make_unique<IntValueType>(0, 1, 0, 5));
	_types.back().AddInput(std::make_unique<PinType>("ptch", "Pitch", PinType::ConnectionType::Single, Engine::Pin::LFO::Input::Pitch)).SetValueType(std::make_unique<PitchValueType>());
	_types.back().AddInput(std::make_unique<PinType>("levl", "Level", PinType::ConnectionType::Single, Engine::Pin::LFO::Input::Level)).SetValueType(std::make_unique<PercentValueType>(0, 100, 0, 1000));
	_types.back().AddInput(std::make_unique<PinType>("duty", "Duty", PinType::ConnectionType::Single, Engine::Pin::LFO::Input::Duty)).SetValueType(std::make_unique<PercentValueType>(50, 25, 0, 100));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, Engine::Pin::LFO::Output::Signal)).SetMonitor();

	_types.emplace_back("crsh", "Crush", Engine::ModuleType::Crush);
	_types.back().AddInput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, Engine::Pin::Crush::Input::Signal));
	_types.back().AddInput(std::make_unique<PinType>("freq", "Frequency", PinType::ConnectionType::Single, Engine::Pin::Crush::Input::Frequency)).SetValueType(std::make_unique<PercentValueType>(100, 100, 1, 100));
	_types.back().AddInput(std::make_unique<PinType>("ampl", "Amplitude", PinType::ConnectionType::Single, Engine::Pin::Crush::Input::Amplitude)).SetValueType(std::make_unique<PercentValueType>(100, 100, 1, 100));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, Engine::Pin::Crush::Output::Signal));

	_types.emplace_back("dely", "Delay", Engine::ModuleType::Delay);
	_types.back().AddInput(std::make_unique<PinType>("peri", "Period", PinType::ConnectionType::Single, Engine::Pin::Delay::Input::Period)).SetValueType(std::make_unique<TimeValueType>(250));
	_types.back().AddInput(std::make_unique<PinType>("fbck", "Feedback", PinType::ConnectionType::Single, Engine::Pin::Delay::Input::Feedback)).SetValueType(std::make_unique<PercentValueType>(50, 50, 0, 100));
	_types.back().AddInput(std::make_unique<PinType>("wet_", "Wet", PinType::ConnectionType::Single, Engine::Pin::Delay::Input::Wet)).SetValueType(std::make_unique<PercentValueType>(100, 100, 0, 100));
	_types.back().AddInput(std::make_unique<PinType>("dry_", "Dry", PinType::ConnectionType::Single, Engine::Pin::Delay::Input::Dry)).SetValueType(std::make_unique<PercentValueType>(100, 100, 0, 100));
	_types.back().AddInput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, Engine::Pin::Delay::Input::Signal)).SetValueType(std::make_unique<PercentValueType>(0, 100, -100, 100));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, Engine::Pin::Delay::Output::Signal));

	_types.emplace_back("knob", "Knob", Engine::ModuleType::Knob);
	for (int i = 0; i < Engine::Pin::Knob::Output::_Count; ++i)
	{
		_types.back().AddInput(std::make_unique<PinType>(Kernel::FormatString("rem%0", i), Kernel::FormatString("Remote %0", i), PinType::ConnectionType::Single, i)).SetValueType(std::make_unique<PercentValueType>(0, 0, 0, 100, 5));
		_types.back().AddOutput(std::make_unique<PinType>(Kernel::FormatString("val%0", i), Kernel::FormatString("Value %0", i), PinType::ConnectionType::Single, i));
	}

	_types.emplace_back("sqnc", "Sequence", Engine::ModuleType::Sequence);
	_types.back().AddField(std::make_unique<FieldType>("sqnc", Engine::Pin::Sequence::Field::Sequence));
	_types.back().AddInput(std::make_unique<PinType>("ptch", "Pitch", PinType::ConnectionType::Single, Engine::Pin::Sequence::Input::Pitch)).SetValueType(std::make_unique<PitchValueType>());
	_types.back().AddOutput(std::make_unique<PinType>("valu", "Value", PinType::ConnectionType::Single, Engine::Pin::Sequence::Output::Value));
	_types.back().AddOutput(std::make_unique<PinType>("gate", "Gate", PinType::ConnectionType::Single, Engine::Pin::Sequence::Output::Gate));
}
	
const ModuleType* ModuleTypes::Find(Tag id)
{
	for (auto& type : Instance()._types)
		if (type.GetID() == id)
			return &type;
	return nullptr;
}