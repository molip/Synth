#include "MIDIModule.h"

#define MIDI_NOTE_OFF 128
#define MIDI_NOTE_ON 144

void MIDIModule::ProcessMIDI(byte midiByte)
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

void MIDIModule::StartNote(byte midiNote)
{
	_pitchOutput.SetValue(midiNote << 9); // To 16 bit.
	_gateOutput.SetValue(UnsignedMax, true);
	_midiNotePlaying = midiNote;
}

void MIDIModule::StopNote(byte midiNote)
{
	if (_midiNote == _midiNotePlaying) // Avoid stopping note when releasing an old one.
	{
		_gateOutput.SetValue(0, true);
		_midiNotePlaying = 0;
	}
}
