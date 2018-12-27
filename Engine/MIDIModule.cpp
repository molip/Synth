#include "MIDIModule.h"
#include "Util.h"

#include <limits.h>

using namespace Engine;

#define MIDI_NOTE_OFF 128
#define MIDI_NOTE_ON 144

MIDIModule::MIDIModule(int polyphony) : _arpeggiator(polyphony, _multiOutputs)
{
	_notes.SetSize(polyphony);
	_multiOutputs.SetSize(Pin::MIDI::MultiOutput::_Count);
	_rawOutputs.SetSize(Pin::MIDI::MultiOutput::_Count);

	for (int i = 0; i < _rawOutputs.GetSize(); ++i)
		_rawOutputs[i].SetSize(polyphony);

	_arpeggiator.Connect(_rawOutputs);
	_useOutputs = &_multiOutputs;
}

void MIDIModule::UpdateArpeggiator()
{
	if (_settings.arpEnabled)
		_arpeggiator.Update();
}

void MIDIModule::ProcessMIDI(int8_t midiByte)
{
	if (midiByte & 0x80)
	{
		// remove channel info
		//byte midiChannel = midiByte & 0xf;
		_midiCommand = midiByte & 0xf0;
		_midiNote = 0;

		//SERIAL_PRINT("Command: "); SERIAL_PRINT(_midiCommand); SERIAL_PRINT("\n");
	}
	else
	{
		if (_midiCommand == MIDI_NOTE_ON)
		{
			if (_midiNote == 0)
			{
				_midiNote = midiByte;
				//SERIAL_PRINT("Note: "); SERIAL_PRINTLN(_midiNote);
			}
			else
			{
				byte velocity = midiByte;
				
				if (velocity > 0)
					StartNote(_midiNote);
				else
					StopNote(_midiNote);

				_midiNote = 0;
				//SERIAL_PRINT("Velocity: "); SERIAL_PRINT(velocity); SERIAL_PRINT("\n");
			}
		}
		else if (_midiCommand == MIDI_NOTE_OFF)
		{
			if (_midiNote == 0)
			{
				_midiNote = midiByte;
				//SERIAL_PRINT("Note: "); SERIAL_PRINTLN(_midiNote);
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
		(*_useOutputs)[Pin::MIDI::MultiOutput::Gate][i].SetValue(0, true);
		_notes[i].midiNote = -1;
		_notes[i].order = 0;
		_startCount = _endCount = 0;
	}
}

void MIDIModule::SetAllNotesOn()
{
	ResetMIDI();

	for (byte i = 0; i < _notes.GetSize(); ++i)
		StartNote(60 + i);
}

void MIDIModule::SetSettings(const Settings& settings)
{
	if (_settings.arpEnabled != settings.arpEnabled)
	{
		_useOutputs = settings.arpEnabled ? &_rawOutputs : &_multiOutputs;
		ResetMIDI();
	}

	if (settings.arpPeriod != _settings.arpPeriod)
		_arpeggiator.SetPeriod(settings.arpPeriod);

	if (settings.arpOctaves != _settings.arpOctaves)
		_arpeggiator.SetOctaves(settings.arpOctaves);

	_settings = settings;
}

void MIDIModule::StartNote(int8_t midiNote)
{
	int index = FindNote(midiNote); // The controller shouldn't start a note twice, but we should handle it anyway. 
	if (index < 0)
		index = FindOldestNote();

	//SERIAL_PRINT("Starting note: "); SERIAL_PRINTLN(index);

	Note& note = _notes[index];
	note.midiNote = midiNote;
	(*_useOutputs)[Pin::MIDI::MultiOutput::Pitch][index].SetValue(midiNote);
	(*_useOutputs)[Pin::MIDI::MultiOutput::Gate][index].SetValue(1, true);
	note.order = ++_startCount;
}

void MIDIModule::StopNote(int8_t midiNote)
{
	int index = FindNote(midiNote);
	if (index >= 0)
	{
		//SERIAL_PRINT("Stopping note: "); SERIAL_PRINTLN(index);

		(*_useOutputs)[Pin::MIDI::MultiOutput::Gate][index].SetValue(0, true);
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
