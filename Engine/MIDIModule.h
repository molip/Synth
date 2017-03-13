#pragma once 
#include "Module.h"
#include "Array.h"

class MIDIModule : public Module
{
public:
	MIDIModule(int polyphony);
	void ResetChanged();

	void ProcessMIDI(int8_t midiByte);
	void ResetMIDI();

	class Note 
	{
	public:
		int midiNote = -1;
		unsigned long order = 0; // Start order if active, otherwise end order. 
	};

	Array<Note> _notes;

private:
	void StartNote(int8_t midiNote);
	void StopNote(int8_t midiNote);
	int FindNote(int8_t midiNote) const;
	int FindOldestNote() const;

	byte _midiCommand = 0;
	byte _midiNote = 0;

	unsigned long _startCount = 0;
	unsigned long _endCount = 0;
};
