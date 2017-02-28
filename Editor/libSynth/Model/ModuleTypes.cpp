#include "stdafx.h"
#include "ModuleTypes.h"

using namespace Synth::Model;

const ModuleTypes& ModuleTypes::Instance()
{
	static ModuleTypes instance;
	return instance;
}

ModuleTypes::ModuleTypes()
{
	_types.emplace_back("midi", "MIDI", Engine::ModuleType::MIDI);
	_types.back().AddOutput(PinType("gate", "Gate", PinType::ConnectionType::Multi, PinType::DataType::Unsigned, (int)Engine::Pin::MIDI::UnsignedMultiOutput::Gate));
	_types.back().AddOutput(PinType("ptch", "Pitch", PinType::ConnectionType::Multi, PinType::DataType::Unsigned, Engine::Pin::MIDI::UnsignedMultiOutput::Pitch));

	_types.emplace_back("envl", "Envelope", Engine::ModuleType::Envelope);
	_types.back().AddInput(PinType("gate", "Gate", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Envelope::UnsignedInput::Gate));
	_types.back().AddInput(PinType("atck", "Attack", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Envelope::UnsignedInput::Attack));
	_types.back().AddInput(PinType("decy", "Decay", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Envelope::UnsignedInput::Decay));
	_types.back().AddInput(PinType("sust", "Sustain", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Envelope::UnsignedInput::Sustain));
	_types.back().AddInput(PinType("rels", "Release", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Envelope::UnsignedInput::Release));
	_types.back().AddOutput(PinType("levl", "Level", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Envelope::UnsignedOutput::Level));
		
	_types.emplace_back("oscl", "Oscillator", Engine::ModuleType::Oscillator);
	_types.back().AddInput(PinType("ptch", "Pitch", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Oscillator::UnsignedInput::Pitch));
	_types.back().AddInput(PinType("levl", "Level", PinType::ConnectionType::Single, PinType::DataType::Unsigned, Engine::Pin::Oscillator::UnsignedInput::Level));
	_types.back().AddOutput(PinType("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Oscillator::SignedOutput::Signal));

	_types.emplace_back("pmix", "PolyMixer", Engine::ModuleType::Mixer);
	_types.back().AddInput(PinType("sgnl", "Signal", PinType::ConnectionType::Multi, PinType::DataType::Signed, Engine::Pin::PolyMixer::SignedMultiInput::Signal));
	_types.back().AddOutput(PinType("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::PolyMixer::SignedOutput::Signal));

	_types.emplace_back("trgt", "Target", Engine::ModuleType::Target);
	_types.back().AddInput(PinType("sgnl", "Signal", PinType::ConnectionType::Single, PinType::DataType::Signed, Engine::Pin::Target::SignedInput::Signal));
}
	
const ModuleType* ModuleTypes::Find(Tag id)
{
	for (auto& type : Instance()._types)
		if (type.GetID() == id)
			return &type;
	return nullptr;
}