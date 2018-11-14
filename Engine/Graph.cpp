#include "Graph.h"

#include "EnvelopeModule.h"
#include "MIDIModule.h"
#include "OscillatorModule.h"
#include "PolyMixerModule.h"
#include "MixerModule.h"
#include "OutputModule.h"
#include "FilterModule.h"
#include "RangeModule.h"
#include "PitchShiftModule.h"
#include "LFOModule.h"
#include "DelayModule.h"
#include "ArpeggiatorModule.h"

using namespace Engine;

Graph* Graph::_active;

void Graph::Init(byte modCount, byte polyphony)
{
	_modules.Reserve(modCount);
	_updateModules.Reserve(modCount); // Won't all be used. 
	_modIndices.Reserve(modCount);
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

void Graph::Update()
{
	for (int i = 0; i < _updateModules.GetSize(); ++i)
		_updateModules[i]->Update();
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
	case ModuleType::PolyMixer: mod = new PolyMixerModule(_polyphony); break;
	case ModuleType::Mixer: mod = new MixerModule; break;
	case ModuleType::Target: mod = new OutputModule; break;
	case ModuleType::Filter: mod = new FilterModule; break;
	case ModuleType::Range: mod = new RangeModule; break;
	case ModuleType::PitchShift: mod = new PitchShiftModule; break;
	case ModuleType::LFO: mod = new LFOModule; break;
	case ModuleType::Delay: mod = new DelayModule; break;
	case ModuleType::Arpeggiator: mod = new ArpeggiatorModule(_polyphony); break;
	default: return;
	}

	mod->SetPolyphony(_polyphony);

	_modIndices.Push(_modules.GetSize());
	_modules.Push(mod);

	if (mod->WantUpdate())
		_updateModules.Push(mod);
}

Module * Graph::GetModule(byte modIndex)
{
	if (modIndex >= _modIndices.GetSize())
		return nullptr;

	return _modules[_modIndices[modIndex]];
}
