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
	_types.back().AddOutput(std::make_unique<PinType>("gate", "Gate", PinType::ConnectionType::Multi, PinType::DataType::Unsigned, (int)Engine::Pin::MIDI::UnsignedMultiOutput::Gate));
	_types.back().AddOutput(std::make_unique<PinType>("ptch", "Pitch", PinType::ConnectionType::Multi, PinType::DataType::Unsigned, Engine::Pin::MIDI::UnsignedMultiOutput::Pitch));

	_types.emplace_back("envl", "Envelope", Engine::ModuleType::Envelope);
	_types.back().AddInput(std::make_unique<PinType>("gate", "Gate", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Envelope::UnsignedInput::Gate));
	_types.back().AddInput(std::make_unique<PinType>("atck", "Attack", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Envelope::UnsignedInput::Attack)).SetValueType(std::make_unique<TimeValueType>(50));
	_types.back().AddInput(std::make_unique<PinType>("decy", "Decay", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Envelope::UnsignedInput::Decay)).SetValueType(std::make_unique<TimeValueType>(50));
	_types.back().AddInput(std::make_unique<PinType>("sust", "Sustain", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Envelope::UnsignedInput::Sustain)).SetValueType(std::make_unique<PercentValueType>(0x8000));
	_types.back().AddInput(std::make_unique<PinType>("rels", "Release", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Envelope::UnsignedInput::Release)).SetValueType(std::make_unique<TimeValueType>(400));
	_types.back().AddOutput(std::make_unique<PinType>("levl", "Level", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Envelope::UnsignedOutput::Level));
		
	_types.emplace_back("oscl", "Oscillator", Engine::ModuleType::Oscillator);
	_types.back().AddInput(std::make_unique<PinType>("ptch", "Pitch", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Oscillator::UnsignedInput::Pitch)).SetValueType(std::make_unique<PercentValueType>(0x8000));
	_types.back().AddInput(std::make_unique<PinType>("levl", "Level", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Oscillator::UnsignedInput::Level)).SetValueType(std::make_unique<PercentValueType>(0x8000));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Oscillator::SignedOutput::Signal));

	_types.emplace_back("pmix", "PolyMixer", Engine::ModuleType::Mixer);
	_types.back().AddInput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Multi, PinType::DataType::Signed, Engine::Pin::PolyMixer::SignedMultiInput::Signal));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::PolyMixer::SignedOutput::Signal));

	_types.emplace_back("trgt", "Target", Engine::ModuleType::Target);
	_types.back().AddInput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Target::SignedInput::Signal));

	_types.emplace_back("filt", "Filter", Engine::ModuleType::Filter);
	_types.back().AddInput(std::make_unique<PinType>("freq", "Frequency", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Filter::UnsignedInput::Frequency)).SetValueType(std::make_unique<PercentValueType>(0x8000));
	_types.back().AddInput(std::make_unique<PinType>("resn", "Resonance", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Filter::UnsignedInput::Resonance)).SetValueType(std::make_unique<PercentValueType>(0x8000));
	_types.back().AddInput(std::make_unique<PinType>("byps", "Bypass", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Filter::UnsignedInput::Bypass)).SetValueType(std::make_unique<BoolValueType>(0));
	_types.back().AddInput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Filter::SignedInput::Signal));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Filter::SignedOutput::Signal));

	_types.emplace_back("rang", "Range", Engine::ModuleType::Range);
	_types.back().AddInput(std::make_unique<PinType>("valu", "Value", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Range::UnsignedInput::Value));
	_types.back().AddInput(std::make_unique<PinType>("mini", "Minimum", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Range::UnsignedInput::Minimum)).SetValueType(std::make_unique<PercentValueType>(0));
	_types.back().AddInput(std::make_unique<PinType>("maxi", "Maximum", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Range::UnsignedInput::Maximum)).SetValueType(std::make_unique<PercentValueType>(0xffff));
	_types.back().AddOutput(std::make_unique<PinType>("valu", "Value", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Range::UnsignedOutput::Value));
}
	
const ModuleType* ModuleTypes::Find(Tag id)
{
	for (auto& type : Instance()._types)
		if (type.GetID() == id)
			return &type;
	return nullptr;
}