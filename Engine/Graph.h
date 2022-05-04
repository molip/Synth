#pragma once 

#include "Array.h"
#include "Module.h"

#include "Platform.h"

namespace Engine
{
class Module;
class MIDIModule;
class KnobModule;
class OutputModule;

class Graph
{
public:
	void Init(byte modCount, byte polyModCount, byte polyphony);
	void AddMonoModule(ModuleType type);
	void AddPolyModule(ModuleType type);

	Module* GetMonoModule(byte modIndex);
	Module* GetPolyModule(byte modIndex, byte instanceIndex);
	
	int GetPolyphony() const { return _polyphony; }

	void ProcessMIDI(byte data);
	void ResetMIDI();
	void SetAllNotesOn();
	void SetSettings(const Settings& settings);
	float Update();
	
	int GetKnobCount() const;
	void UpdateKnob(int index, uint16_t val);
	void UpdateKnobsRemote();

	static Graph* GetActive() { return _active; }
	void Activate();

#ifdef _WIN32
	using MonitorLevels = std::vector<std::vector<float>>;
	MonitorLevels GetMonitorLevels() const;
	void AddMonitor(bool mono, int modIndex, int pinIndex);
#else
	void AddMonitor(bool mono, int modIndex, int pinIndex) {}
#endif

private:
	void AddModule(ModuleType type);

	struct Monitor
	{
		bool mono;
		int modIndex;
		int pinIndex;
	};

	static Graph* _active;

	Array<uint16_t> _modIndices;
	Array<uint16_t> _polyModIndices; // Indices of first instance.
	PtrArray<Module> _modules;
	Array<Module*> _updateModules;
	Array<Monitor> _monitors;
	byte _polyphony = 0;
	MIDIModule* _midiModule = nullptr;
	OutputModule* _outputModule = nullptr;
	KnobModule* _knobModule = nullptr;
};

}