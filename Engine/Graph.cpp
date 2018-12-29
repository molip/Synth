#include "Graph.h"

#include "EnvelopeModule.h"
#include "MIDIModule.h"
#include "OscillatorModule.h"
#include "PolyMixerModule.h"
#include "MixerModule.h"
#include "MultiplyModule.h"
#include "OutputModule.h"
#include "FilterModule.h"
#include "MathsModule.h"
#include "LFOModule.h"
#include "DelayModule.h"
#include "ArpeggiatorModule.h"
#include "KnobModule.h"

using namespace Engine;

Graph* Graph::_active;

void Graph::Init(byte modCount, byte polyModCount, byte polyphony)
{
	const int instanceModCount = polyModCount * polyphony;
	_modules.Reserve(modCount + instanceModCount);
	_updateModules.Reserve(modCount + instanceModCount); // Won't all be used. 
	_knobModules.Reserve(8);
	_modIndices.Reserve(modCount);
	_polyModIndices.Reserve(instanceModCount);
	_polyphony = polyphony;
}

void Graph::ProcessMIDI(byte data)
{
	if (_midiModule)
		_midiModule->ProcessMIDI(data);
}

void Graph::ResetMIDI()
{
	if (_midiModule)
		_midiModule->ResetMIDI();
}

void Graph::SetAllNotesOn()
{
	if (_midiModule)
		_midiModule->SetAllNotesOn();
}

void Graph::SetSettings(const Settings& settings)
{
	if (_midiModule)
		_midiModule->SetSettings(settings);
}

float Graph::Update()
{
	if (_midiModule)
		_midiModule->UpdateArpeggiator();

	for (int i = 0; i < _updateModules.GetSize(); ++i)
		_updateModules[i]->Update();

	return _outputModule ? _outputModule->GetLevel() : 0;
}

void Graph::UpdateKnob(int index, uint16_t val)
{
	if (index < _knobModules.GetSize())
		_knobModules[index]->SetValue(val);
}

void Graph::UpdateKnobsRemote()
{
	for (int i = 0; i < _knobModules.GetSize(); ++i)
		_knobModules[i]->UpdateRemote();
}

void Graph::Activate()
{
	if (_active == this)
		return;

	delete _active;
	_active = this;

	for (int i = 0; i < _modules.GetSize(); ++i)
		_modules[i]->Initialise();

	// Sort knob modules by index.
	for (int i = 0; i < _knobModules.GetSize() - 1; ++i)
		for (int j = 0; j < _knobModules.GetSize() - j - 1; ++j)
			if (_knobModules[j]->GetIndex() > _knobModules[j + 1]->GetIndex())
			{
				auto temp = _knobModules[j];
				_knobModules[j] = _knobModules[j + 1];
				_knobModules[j + 1] = temp;
			}
}

void Graph::AddModule(ModuleType type)
{
	Module* mod = nullptr;
	KnobModule* knobMod = nullptr;
	switch (type)
	{
	case ModuleType::MIDI: mod = _midiModule = new MIDIModule(_polyphony); break;
	case ModuleType::Envelope: mod = new EnvelopeModule; break;
	case ModuleType::Oscillator: mod = new OscillatorModule; break;
	case ModuleType::PolyMixer: mod = new PolyMixerModule(_polyphony); break;
	case ModuleType::Mixer: mod = new MixerModule; break;
	case ModuleType::Multiply: mod = new MultiplyModule; break;
	case ModuleType::Target: mod = _outputModule = new OutputModule; break;
	case ModuleType::Filter: mod = new FilterModule; break;
	case ModuleType::Maths: mod = new MathsModule; break;
	case ModuleType::LFO: mod = new LFOModule; break;
	case ModuleType::Delay: mod = new DelayModule; break;
	case ModuleType::Knob: mod = knobMod = new KnobModule(); break;
	default: return;
	}

	mod->SetPolyphony(_polyphony);

	_modules.Push(mod);

	if (knobMod)
		_knobModules.Push(knobMod);

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
