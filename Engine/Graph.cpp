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
#include "CrushModule.h"
#include "DelayModule.h"
#include "ArpeggiatorModule.h"
#include "KnobModule.h"
#include "SequenceModule.h"

using namespace Engine;

Graph* Graph::_active;

void Graph::Init(byte modCount, byte polyModCount, byte polyphony)
{
	const int instanceModCount = polyModCount * polyphony;
	_modules.Reserve(modCount + instanceModCount);
	_updateModules.Reserve(modCount + instanceModCount); // Won't all be used. 
	_modIndices.Reserve(modCount);
	_polyModIndices.Reserve(instanceModCount);
	_monitors.Reserve(10);
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

int Graph::GetKnobCount() const 
{ 
	return _knobModule ? _knobModule->GetActiveCount() : 0; 
}

void Graph::UpdateKnob(int index, uint16_t val)
{
	_knobModule->SetValue(index, val);
}

void Graph::UpdateKnobsRemote()
{
	if (_knobModule)
		_knobModule->UpdateRemote();
}

void Graph::Activate()
{
	if (_active == this)
		return;

	delete _active;
	_active = this;

	for (int i = 0; i < _modules.GetSize(); ++i)
		_modules[i]->Initialise();
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
	case ModuleType::Crush: mod = new CrushModule; break;
	case ModuleType::Delay: mod = new DelayModule; break;
	case ModuleType::Knob: mod = knobMod = new KnobModule(); break;
	case ModuleType::Sequence: mod = new SequenceModule(); break;
	default: return;
	}

	mod->SetPolyphony(_polyphony);

	_modules.Push(mod);

	if (knobMod)
		_knobModule = knobMod;

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

#ifdef _WIN32
void Graph::AddMonitor(bool mono, int modIndex, int pinIndex)
{
	if (!_monitors.IsFull())
		_monitors.Push({ mono, modIndex, pinIndex });
}

Graph::MonitorLevels Graph::GetMonitorLevels() const
{
	MonitorLevels levels;
	levels.resize(_monitors.GetSize());
	for (int i = 0; i < _monitors.GetSize(); ++i)
	{
		auto& monitor = _monitors[i];

		auto addLevel = [&](const Module* mod)
		{
			levels[i].push_back(mod->GetPins<Output>()[monitor.pinIndex].GetValue());
		};

		if (monitor.mono)
			addLevel(const_cast<Graph*>(this)->GetMonoModule(monitor.modIndex));
		else
		{
			levels[i].reserve(_polyphony);
			for (int i = 0; i < _polyphony; ++i)
				addLevel(const_cast<Graph*>(this)->GetPolyModule(monitor.modIndex, i));
		}
	}

	return levels;
}

#endif