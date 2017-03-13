#include "MIDIModule.h"

#include <limits.h>

#define MIDI_NOTE_OFF 128
#define MIDI_NOTE_ON 144

MIDIModule::MIDIModule(int polyphony)
{
	_notes.SetSize(polyphony);
	_unsignedMultiOutputs.SetSize(Pin::MIDI::UnsignedMultiOutput::_Count);
}

void MIDIModule::ResetChanged() 
{
	int count = _unsignedMultiOutputs[Pin::MIDI::UnsignedMultiOutput::Gate].GetSize();
	for (int i = 0; i < count; ++i)
	{
		_unsignedMultiOutputs[Pin::MIDI::UnsignedMultiOutput::Gate][i].ResetChanged();
		_unsignedMultiOutputs[Pin::MIDI::UnsignedMultiOutput::Pitch][i].ResetChanged();
	}
}

void MIDIModule::ProcessMIDI(int8_t midiByte)
{
	if (midiByte & 0x80)
	{
		// remove channel info
		//byte midiChannel = midiByte & 0xf;
		_midiCommand = midiByte & 0xf0;
		_midiNote = 0;

		//Serial.print("Command: "); Serial.print(_midiCommand); Serial.print("\n");
	}
	else
	{
		if (_midiCommand == MIDI_NOTE_ON)
		{
			if (_midiNote == 0)
			{
				_midiNote = midiByte;
				//Serial.print("Note: "); Serial.println(_midiNote);
			}
			else
			{
				byte velocity = midiByte;
				
				if (velocity > 0)
					StartNote(_midiNote);
				else
					StopNote(_midiNote);

				_midiNote = 0;
				//Serial.print("Velocity: "); Serial.print(velocity); Serial.print("\n");
			}
		}
		else if (_midiCommand == MIDI_NOTE_OFF)
		{
			if (_midiNote == 0)
			{
				_midiNote = midiByte;
				//Serial.print("Note: "); Serial.println(_midiNote);
			}
			else
			{
				StopNote(_midiNote);
				_midiNote = 0;
			}
		}
	}
}

void MIDIModule::ResetMIDI()
{
	_midiCommand = _midiNote = 0;

	for (int i = 0; i < _notes.GetSize(); ++i)
	{
		_unsignedMultiOutputs[Pin::MIDI::UnsignedMultiOutput::Gate][i].SetValue(0, true);
		_notes[i].midiNote = -1;
		_notes[i].order = 0;
		_startCount = _endCount = 0;
	}
}

void MIDIModule::StartNote(int8_t midiNote)
{
	int index = FindNote(midiNote); // The controller shouldn't start a note twice, but we should handle it anyway. 
	if (index < 0)
		index = FindOldestNote();

	//Serial.print("Starting note: "); Serial.println(index);

	Note& note = _notes[index];
	note.midiNote = midiNote;
	_unsignedMultiOutputs[Pin::MIDI::UnsignedMultiOutput::Pitch][index].SetValue(midiNote << 9); // To 16 bit.
	_unsignedMultiOutputs[Pin::MIDI::UnsignedMultiOutput::Gate][index].SetValue(UnsignedMax, true);
	note.order = ++_startCount;
}

void MIDIModule::StopNote(int8_t midiNote)
{
	int index = FindNote(midiNote);
	if (index >= 0)
	{
		//Serial.print("Stopping note: "); Serial.println(index);

		_unsignedMultiOutputs[Pin::MIDI::UnsignedMultiOutput::Gate][index].SetValue(0, true);
		_notes[index].midiNote = -1;
		_notes[index].order = ++_endCount;
	}
}

int MIDIModule::FindNote(int8_t midiNote) const
{
	for (int i = 0; i < _notes.GetSize(); ++i) 
		if (_notes[i].midiNote == midiNote)
			return i;
	return -1;
}

int MIDIModule::FindOldestNote() const
{
	unsigned long oldestEnd = ULONG_MAX;

	// Find first ended. 
	int index = -1;
	for (int i = 0; i < _notes.GetSize(); ++i) 
		if (_notes[i].midiNote < 0 && oldestEnd > _notes[i].order)
			oldestEnd = _notes[i].order, index = i;
	
	if (index >= 0) 
		return index;

	// They're all still active; find first started. 
	unsigned long oldestStart = ULONG_MAX;
	for (int i = 0; i < _notes.GetSize(); ++i) 
		if (_notes[i].midiNote >= 0 && oldestStart > _notes[i].order)
			oldestStart = _notes[i].order, index = i;

	return index;
}
