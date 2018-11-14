#include "stdafx.h"
#include "ModuleTypes.h"

#include "../../libKernel/Debug.h"

using namespace Synth::Model;

const ModuleTypes& ModuleTypes::Instance()
{
	static ModuleTypes instance;
	return instance;
}

ModuleTypes::ModuleTypes()
{
	_types.emplace_back("midi", "MIDI", Engine::ModuleType::MIDI);
	_types.back().AddOutput(std::make_unique<PinType>("ptch", "Pitch", PinType::ConnectionType::Multi, PinType::DataType::Signed, Engine::Pin::MIDI::MultiOutput::Pitch));
	_types.back().AddOutput(std::make_unique<PinType>("gate", "Gate", PinType::ConnectionType::Multi, PinType::DataType::Signed, Engine::Pin::MIDI::MultiOutput::Gate));

	_types.emplace_back("envl", "Envelope", Engine::ModuleType::Envelope);
	_types.back().AddInput(std::make_unique<PinType>("gate", "Gate", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Envelope::Input::Gate));
	_types.back().AddInput(std::make_unique<PinType>("atck", "Attack", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Envelope::Input::Attack)).SetValueType(std::make_unique<TimeValueType>(50));
	_types.back().AddInput(std::make_unique<PinType>("decy", "Decay", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Envelope::Input::Decay)).SetValueType(std::make_unique<TimeValueType>(50));
	_types.back().AddInput(std::make_unique<PinType>("sust", "Sustain", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Envelope::Input::Sustain)).SetValueType(std::make_unique<PercentValueType>(0x8000));
	_types.back().AddInput(std::make_unique<PinType>("rels", "Release", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Envelope::Input::Release)).SetValueType(std::make_unique<TimeValueType>(400));
	_types.back().AddOutput(std::make_unique<PinType>("levl", "Level", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Envelope::Output::Level));
		
	_types.emplace_back("oscl", "Oscillator", Engine::ModuleType::Oscillator);
	_types.back().AddInput(std::make_unique<PinType>("wave", "Waveform", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Oscillator::Input::Waveform)).SetValueType(std::make_unique<IntValueType>(0, 0, 3));
	_types.back().AddInput(std::make_unique<PinType>("ptch", "Pitch", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Oscillator::Input::Pitch)).SetValueType(std::make_unique<PercentValueType>(0x8000));
	_types.back().AddInput(std::make_unique<PinType>("levl", "Level", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Oscillator::Input::Level)).SetValueType(std::make_unique<PercentValueType>(0x8000));
	_types.back().AddInput(std::make_unique<PinType>("pmod", "PhaseMod", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Oscillator::Input::PhaseMod));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Oscillator::Output::Signal));

	_types.emplace_back("pmix", "PolyMixer", Engine::ModuleType::PolyMixer);
	_types.back().AddInput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Multi, PinType::DataType::Signed, Engine::Pin::PolyMixer::MultiInput::Signal));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::PolyMixer::Output::Signal));

	_types.emplace_back("trgt", "Target", Engine::ModuleType::Target);
	_types.back().AddInput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Target::Input::Signal));

	_types.emplace_back("filt", "Filter", Engine::ModuleType::Filter);
	_types.back().AddInput(std::make_unique<PinType>("freq", "Frequency", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Filter::Input::Frequency)).SetValueType(std::make_unique<PercentValueType>(0x8000));
	_types.back().AddInput(std::make_unique<PinType>("resn", "Resonance", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Filter::Input::Resonance)).SetValueType(std::make_unique<PercentValueType>(0x8000));
	_types.back().AddInput(std::make_unique<PinType>("byps", "Bypass", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Filter::Input::Bypass)).SetValueType(std::make_unique<BoolValueType>(0));
	_types.back().AddInput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Filter::Input::Signal));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Filter::Output::Signal));

	_types.emplace_back("rang", "Range", Engine::ModuleType::Range);
	_types.back().AddInput(std::make_unique<PinType>("valu", "Value", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Range::Input::Value));
	_types.back().AddInput(std::make_unique<PinType>("mini", "Minimum", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Range::Input::Minimum)).SetValueType(std::make_unique<PercentValueType>(0));
	_types.back().AddInput(std::make_unique<PinType>("maxi", "Maximum", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Range::Input::Maximum)).SetValueType(std::make_unique<PercentValueType>(0xffff));
	_types.back().AddOutput(std::make_unique<PinType>("valu", "Value", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Range::Output::Value));

	_types.emplace_back("mixr", "Mixer", Engine::ModuleType::Mixer);
	_types.back().AddInput(std::make_unique<PinType>("sig0", "Signal 0", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Mixer::Input::Signal0));
	_types.back().AddInput(std::make_unique<PinType>("sig1", "Signal 1", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Mixer::Input::Signal1));
	_types.back().AddInput(std::make_unique<PinType>("sig2", "Signal 2", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Mixer::Input::Signal2));
	_types.back().AddInput(std::make_unique<PinType>("sig3", "Signal 3", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Mixer::Input::Signal3));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Mixer::Output::Signal));

	_types.emplace_back("pitc", "PitchShift", Engine::ModuleType::PitchShift);
	_types.back().AddInput(std::make_unique<PinType>("shft", "Shift", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::PitchShift::Input::Shift)).SetValueType(std::make_unique<IntValueType>(0, -127, 127));
	_types.back().AddInput(std::make_unique<PinType>("pitc", "Pitch", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::PitchShift::Input::Pitch));
	_types.back().AddOutput(std::make_unique<PinType>("pitc", "Pitch", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::PitchShift::Output::Pitch));

	_types.emplace_back("lfo", "LFO", Engine::ModuleType::LFO);
	_types.back().AddInput(std::make_unique<PinType>("trig", "Trigger", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::LFO::Input::Trigger));
	_types.back().AddInput(std::make_unique<PinType>("wave", "Waveform", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::LFO::Input::Waveform)).SetValueType(std::make_unique<IntValueType>(0, 0, 3));
	_types.back().AddInput(std::make_unique<PinType>("ptch", "Pitch", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::LFO::Input::Pitch)).SetValueType(std::make_unique<PercentValueType>(0x8000));
	_types.back().AddInput(std::make_unique<PinType>("levl", "Level", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::LFO::Input::Level)).SetValueType(std::make_unique<PercentValueType>(0xffff));
	_types.back().AddInput(std::make_unique<PinType>("duty", "Duty", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::LFO::Input::Duty)).SetValueType(std::make_unique<PercentValueType>(0x8000));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::LFO::Output::Signal));

	_types.emplace_back("dely", "Delay", Engine::ModuleType::Delay);
	_types.back().AddInput(std::make_unique<PinType>("peri", "Period", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Delay::Input::Period)).SetValueType(std::make_unique<TimeValueType>(250));
	_types.back().AddInput(std::make_unique<PinType>("fbck", "Feedback", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Delay::Input::Feedback)).SetValueType(std::make_unique<PercentValueType>(0x8000));
	_types.back().AddInput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Delay::Input::Signal));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Delay::Output::Signal));

	_types.emplace_back("arpe", "Arpeggiator", Engine::ModuleType::Arpeggiator);
	_types.back().AddInput(std::make_unique<PinType>("peri", "Period", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Arpeggiator::Input::Period)).SetValueType(std::make_unique<TimeValueType>(250));
	_types.back().AddInput(std::make_unique<PinType>("octa", "Octaves", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Arpeggiator::Input::Octaves)).SetValueType(std::make_unique<IntValueType>(1, 1, 8));
	_types.back().AddInput(std::make_unique<PinType>("gate", "Gate", PinType::ConnectionType::Multi, PinType::DataType::Signed, Engine::Pin::Arpeggiator::MultiInput::Gate));
	_types.back().AddInput(std::make_unique<PinType>("pitc", "Pitch", PinType::ConnectionType::Multi, PinType::DataType::Signed, Engine::Pin::Arpeggiator::MultiInput::Pitch));
	_types.back().AddOutput(std::make_unique<PinType>("gate", "Gate", PinType::ConnectionType::Multi, PinType::DataType::Signed, Engine::Pin::Arpeggiator::MultiOutput::Gate));
	_types.back().AddOutput(std::make_unique<PinType>("pitc", "Pitch", PinType::ConnectionType::Multi, PinType::DataType::Signed, Engine::Pin::Arpeggiator::MultiOutput::Pitch));
}
	
const ModuleType* ModuleTypes::Find(Tag id)
{
	for (auto& type : Instance()._types)
		if (type.GetID() == id)
			return &type;
	return nullptr;
}