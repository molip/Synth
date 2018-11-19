#pragma once 

#include "Array.h"
#include "Module.h"

#include "Platform.h"

namespace Engine
{
class Module;
class MIDIModule;
class KnobModule;

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
	void Update();
	
	int GetKnobCount() const { return _knobModules.GetSize(); }
	void UpdateKnob(int index, uint16_t val);

	static Graph* GetActive() { return _active; }
	void Activate();

private:
	void AddModule(ModuleType type);

	static Graph* _active;

	Array<uint16_t> _modIndices;
	Array<uint16_t> _polyModIndices; // Indices of first instance.
	PtrArray<Module> _modules;
	Array<Module*> _updateModules;
	Array<KnobModule*> _knobModules;
	byte _polyphony = 0;
	MIDIModule* _midiModule = nullptr;
};

}