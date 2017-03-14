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
	_types.back().AddInput(std::make_unique<PinType>("ptch", "Pitch", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Oscillator::UnsignedInput::Pitch));
	_types.back().AddInput(std::make_unique<PinType>("levl", "Level", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Oscillator::UnsignedInput::Level));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Oscillator::SignedOutput::Signal));

	_types.emplace_back("pmix", "PolyMixer", Engine::ModuleType::Mixer);
	_types.back().AddInput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Multi, PinType::DataType::Signed, Engine::Pin::PolyMixer::SignedMultiInput::Signal));
	_types.back().AddOutput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::PolyMixer::SignedOutput::Signal));

	_types.emplace_back("trgt", "Target", Engine::ModuleType::Target);
	_types.back().AddInput(std::make_unique<PinType>("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Target::SignedInput::Signal));

	PercentValueType v(0);
	std::string s1 = v.ToString(1);
	std::string s2 = v.ToString(1000);

	int n1 = v.FromString("100%");
	int n2 = v.FromString("50");
	int n3 = v.FromString("1");
}
	
const ModuleType* ModuleTypes::Find(Tag id)
{
	for (auto& type : Instance()._types)
		if (type.GetID() == id)
			return &type;
	return nullptr;
}