#pragma once 

#include "Array.h"
#include "Module.h"

#include "Platform.h"

namespace Engine
{
class Module;
class MIDIModule;

class Graph
{
public:
	void Init(byte modCount, byte polyphony);
	void AddModule(ModuleType type);

	Module* GetModule(byte modIndex);
	
	int GetPolyphony() const { return _polyphony; }

	void ProcessMIDI(byte data);
	void ResetMIDI();
	void Update();

	static Graph* GetActive() { return _active; }
	void Activate();

private:
	static Graph* _active;

	Array<uint16_t> _modIndices;
	Array<uint16_t> _polyModIndices; // Indices of first instance.
	PtrArray<Module> _modules;
	Array<Module*> _updateModules;
	byte _polyphony = 0;
	MIDIModule* _midiModule = nullptr;
};

}