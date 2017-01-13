#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <limits.h>

#include <avr/pgmspace.h>

#include "Types.h"
#include "Sine.h"

#ifdef __AVR_ATmega328P__
	//SoftwareSerial mySerial(2, 3); // RX, TX
	//#define MIDISERIAL mySerial
	#define MIDISERIAL Serial
	#define INIT_DAC() MCP4822_init()
	#define SET_OUTPUT(val)	MCP4822_setOutput(val)
#else
	#define MIDISERIAL Serial2

	#define INIT_DAC() analogWriteResolution(12)
	#define SET_OUTPUT(val) analogWrite(A21, val)
#endif

#define MIDI_NOTE_OFF 128
#define MIDI_NOTE_ON 144

boolean gotTick;

const float pitchMult = 1.059463094359;
float deltas[12];

const unsigned long sampleRate = 50000;

const int lengthBits = 15;
const int length = 1 << lengthBits;

const byte noteBufferBits = 3;
const byte noteBufferLength = 1 << noteBufferBits;
Note noteBuffer[noteBufferLength];

byte midiCommand;
byte midiNote;
 
void setup()
{
	const unsigned long interval = 1000000 / sampleRate;
	const float baseFreq = 16.35; // C0
	
	float delta = baseFreq * 0x10000 / sampleRate;
	for (int i = 0; i < 12; ++i)
	{
		deltas[i] = delta;
		delta *= pitchMult;
	}

	for (int i = 0; i < noteBufferLength; ++i)
		noteBuffer[i].midiNote = -1;
	
	pinMode(4, INPUT_PULLUP);
	pinMode(5, INPUT_PULLUP);
	pinMode(6, INPUT_PULLUP);
	pinMode(7, INPUT_PULLUP);
	
	INIT_DAC();

	Timer1.initialize(interval); 
	Timer1.attachInterrupt(timer);

	Serial.begin(57600);

	MIDISERIAL.begin(31250);
}

void loop()
{
	if (gotTick)
	{
		gotTick = false;
		//unsigned long start = micros();
		DoTick();
		//Serial.println(micros() - start);
	}

	if (MIDISERIAL.available() > 0) 
	{
		byte midiByte = MIDISERIAL.read();

		if (midiByte & 0x80)
		{
			// remove channel info
			//byte midiChannel = midiByte & 0xf;
			midiCommand = midiByte & 0xf0;
			midiNote = 0;

			//Serial.print("Command: "); Serial.print(midiCommand); Serial.print("\n");
		}
		else
		{
			if (midiCommand == MIDI_NOTE_ON)
			{
				if (midiNote == 0)
				{
					midiNote = midiByte;
					//Serial.print("Note: "); Serial.println(midiNote);
				}
				else
				{
					byte type = 0;
					if (!digitalRead(7))
						type = 1;
					else if (!digitalRead(6))
						type = 2;
					else if (!digitalRead(5))
						type = 3;
					
					byte velocity = midiByte;
					if (velocity > 0)
						StartNote(midiNote, type);
					else
						StopNote(midiNote);
					midiNote = 0;
					//Serial.print("Velocity: "); Serial.print(velocity); Serial.print("\n");
				}
			}
			else if (midiCommand == MIDI_NOTE_OFF)
			{
				if (midiNote == 0)
				{
					midiNote = midiByte;
					//Serial.print("Note: "); Serial.println(midiNote);
				}
				else
				{
					byte velocity = midiByte;
					StopNote(midiNote);
					midiNote = 0;
				}
			}
		}
	}
}

// Returns level [-0x800, 0xfff] * (1 << lengthBits).
long ProcessNote(Note& note)
{
	note.phase += note.delta;

	unsigned int phase = note.phase >> 4; // [0, 0xfff]
	int output = 0;
	
	if (note.type == 0) 
		output = pgm_read_byte_near(table + (phase >> 1)) << 4; // Sine.
	else if (note.type == 1)
		output = 2 * ((phase & 0x800) ? 0xfff - phase : phase); // Triangle.
	else if (note.type == 2)
		output = phase; // Sawtooth. 
	else
		output = phase > 0x800 ? 0xfff : 0; // Square.

	return (long)(output - 0x800) * note.ticksLeft;
}

int FindNote(int8_t midiNote)
{
	for (int i = 0; i < noteBufferLength; ++i) 
		if (noteBuffer[i].midiNote == midiNote)
			return i;
	return -1;
}

int FindOldestNote()
{
	unsigned long minTicksLeft = ULONG_MAX;
	int index = 0;
	for (int i = 0; i < noteBufferLength; ++i) 
		if (noteBuffer[i].midiNote >= 0)
			if (minTicksLeft > noteBuffer[i].ticksLeft)
				minTicksLeft = noteBuffer[i].ticksLeft, index = i;
	return index;
}

void StartNote(int8_t midiNote, byte type)
{
	int index = FindNote(midiNote);
	if (index < 0)
		index = FindNote(-1);
	if (index < 0)
		index = FindOldestNote();

	Serial.print("StartNote: midiNote = "); Serial.print((int)midiNote); Serial.print(" index = "); Serial.println(index); 
	
	int octave = midiNote / 12;
	int pitchIndex = midiNote % 12;
	
	float delta = deltas[pitchIndex];
	for (int i = 0; i < octave; ++i)
		delta = delta + delta;

	Note& note = noteBuffer[index];
	note.midiNote = midiNote;
	note.phase = 0;
	note.delta = delta;
	note.ticksLeft = length;
	note.type = type;
}

void StopNote(byte midiNote)
{
	int index = FindNote(midiNote);
	if (index >= 0)
	{
		Note& note = noteBuffer[index];
		note.midiNote = -1;
	}
}

void timer()
{
	gotTick = true;
}

void DoTick()
{
	long output = 0;
	for (int i = 0; i < noteBufferLength; ++i)
	{
		Note& note = noteBuffer[i];
		if (note.midiNote >= 0)
		{
			output += ProcessNote(note);
			if (note.ticksLeft == 0)
			{
				note.midiNote = -1;
			}	
		}
	}	
	
	SET_OUTPUT(0x800 + (output >> (lengthBits + noteBufferBits)));
}
