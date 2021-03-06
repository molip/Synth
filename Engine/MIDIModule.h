#pragma once 
#include "Module.h"
#include "Array.h"
#include "ArpeggiatorModule.h"

namespace Engine
{
class MIDIModule : public Module
{
public:
	MIDIModule(int polyphony);

	void UpdateArpeggiator();

	void ProcessMIDI(int8_t midiByte);
	void ResetMIDI();
	void SetAllNotesOn();
	void SetSettings(const Settings& settings);

	class Note 
	{
	public:
		int midiNote = -1;
		unsigned long order = 0; // Start order if active, otherwise end order. 
	};

private:
	void StartNote(int8_t midiNote);
	void StopNote(int8_t midiNote);
	int FindNote(int8_t midiNote) const;
	int FindOldestNote() const;
	void StopNoteIndex(int index);
	void StopAllNotes();

	Array<Note> _notes;

	byte _midiCommand = 0;
	byte _midiNote = 0;

	unsigned long _startCount = 0;
	unsigned long _endCount = 0;
	unsigned long _pressedCount = 0;

	Array<MultiOutput> _rawOutputs;
	Array<MultiOutput>* _useOutputs;
	ArpeggiatorModule _arpeggiator;

	Settings _settings;
};
}