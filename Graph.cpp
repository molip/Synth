#include "Graph.h"

#include "EnvelopeModule.h"
#include "MIDIModule.h"
#include "OscillatorModule.h"
#include "PolyMixerModule.h"
#include "OutputModule.h"

Graph* Graph::_active;

void Graph::Init(byte modCount, byte polyModCount, byte polyphony)
{
	const int instanceModCount = polyModCount * polyphony;
	_modules.Reserve(modCount + instanceModCount);
	_updateModules.Reserve(modCount + instanceModCount); // Won't all be used. 
	_modIndices.Reserve(modCount);
	_polyModIndices.Reserve(instanceModCount);
	_polyphony = polyphony;
}

void Graph::ProcessMIDI(byte data)
{
	if (_midiModule)
		_midiModule->ProcessMIDI(data);
}

void Graph::Update()
{
	for (int i = 0; i < _updateModules.GetSize(); ++i)
		_updateModules[i]->Update();

	if (_midiModule)
		_midiModule->ResetChanged();
}

void Graph::Activate()
{
	if (_active == this)
		return;

	delete _active;
	_active = this;
}

void Graph::AddModule(ModuleType type)
{
	Module* mod = nullptr;
	switch (type)
	{
	case ModuleType::MIDI: mod = _midiModule = new MIDIModule(_polyphony); break;
	case ModuleType::Envelope: mod = new EnvelopeModule; break;
	case ModuleType::Oscillator: mod = new OscillatorModule; break;
	case ModuleType::Mixer: mod = new PolyMixerModule; break;
	case ModuleType::Target: mod = new OutputModule; break;
	case ModuleType::UnsignedValue: mod = new UnsignedValueModule; break;
	default: return;
	}

	mod->SetPolyphony(_polyphony);

	_modules.Push(mod);

	if (mod->WantUpdate())
		_updateModules.Push(mod);
}

void Graph::AddMonoModule(ModuleType type)
{
	_modIndices.Push(_modules.GetSize());
	AddModule(type);
}

void Graph::AddPolyModule(ModuleType type)
{
	_polyModIndices.Push(_modules.GetSize());
	for (int i = 0; i < _polyphony; ++i)
		AddModule(type);
}

Module * Graph::GetMonoModule(byte modIndex)
{
	if (modIndex >= _modIndices.GetSize())
		return nullptr;

	return _modules[_modIndices[modIndex]];
}

Module * Graph::GetPolyModule(byte modIndex, byte instanceIndex)
{
	if (modIndex >= _polyModIndices.GetSize() || instanceIndex >= _polyphony)
		return nullptr;

	return _modules[_polyModIndices[modIndex] + instanceIndex];
}
