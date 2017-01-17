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
float freqs[12];

const uint32_t sampleRate = 50000;
const float freqToDeltaScale = 0x10000 / (float)sampleRate;

const byte noteBufferBits = 4;
const byte noteBufferLength = 1 << noteBufferBits;
Note noteBuffer[noteBufferLength];

byte midiCommand;
byte midiNote;

uint32_t lastReport;
 
void setup()
{
	const unsigned long interval = 1000000 / sampleRate;
	const float baseFreq = 16.35; // C0
	
	float freq = baseFreq;
	for (int i = 0; i < 12; ++i)
	{
		freqs[i] = freq;
		freq *= pitchMult;
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
		unsigned long start = micros();
		DoTick();
		
		if (start > lastReport + 1000000)
		{	
			lastReport = start;
			Serial.println(micros() - start);
		}
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
					{
						Envelope env;
						env.attack = 0;
						env.decay = 0;
						env.sustain = 1;
						env.release = 1;
						StartNote(midiNote, type, env);
					}
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

// Returns level [-0x800, 0xfff] << 16 (28 bit)
int32_t ProcessNote(Note& note)
{
	uint16_t phaseDelta = note.phaseDelta;

	// FM
	// if (note.mod) 
	// {
		// int32_t phaseOffset = ProcessNote(*note.mod) >> 12; // 16 bit.
		// phaseDelta += phaseOffset * note.modAmount * 0.1; // ?
	// }

	note.phase += phaseDelta;
	++note.ticks;

	uint16_t phase = note.phase;
	uint16_t output = 0;
	
	// PM
	if (note.mod)
	{
		int32_t phaseOffset = ProcessNote(*note.mod) >> 12; // 16 bit.
		phase += phaseOffset * note.modAmount;
	}
	
	phase = phase >> 4; // [0, 0xfff]

	if (note.type == 0) 
		output = pgm_read_byte_near(table + (phase >> 1)) << 4; // Sine.
	else if (note.type == 1)
		output = 2 * ((phase & 0x800) ? 0xfff - phase : phase); // Triangle.
	else if (note.type == 2)
		output = phase; // Sawtooth. 
	else
		output = phase > 0x800 ? 0xfff : 0; // Square.
	
	switch (note.stage)
	{
		case 0:
			if (note.attackDelta && note.level < ULONG_MAX - note.attackDelta)
				note.level += note.attackDelta;
			else
			{
				note.level = ULONG_MAX;
				note.stage = 1;
			}
			break;
		case 1:
			if (note.decayDelta && note.level > note.sustainLevel + note.decayDelta)
				note.level -= note.decayDelta;
			else
			{
				note.level = note.sustainLevel;
				note.stage = 2;
			}
		case 2:
			break;
		case 3:
			if (note.level > note.releaseDelta)
				note.level -= note.releaseDelta;
			else
			{
				note.level = 0;
				note.midiNote = -1;
				delete note.mod;
			}
			break;
	}
	
	//Serial.println(note.level);
	int32_t envOutput = (output - 0x800) * (note.level >> 16);
	return envOutput;
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
	unsigned long maxTicks = 0;
	int index = 0;
	for (int i = 0; i < noteBufferLength; ++i) 
		if (noteBuffer[i].midiNote >= 0)
			if (maxTicks < noteBuffer[i].ticks)
				maxTicks = noteBuffer[i].ticks, index = i;
	return 0;
}

Note* MakeMod(int8_t midiNote, float attack)
{
	Note* mod = new Note;
	Envelope modEnv;
	modEnv.attack = attack;
	modEnv.decay = modEnv.release = 0;
	modEnv.sustain = 1;
	InitNote(*mod, MidiNoteToFrequency(midiNote), 0, modEnv);
}

void StartNote(int8_t midiNote, byte type, const Envelope& env)
{
	unsigned long start = micros();

	int index = FindNote(midiNote);
	if (index < 0)
		index = FindNote(-1);
	if (index < 0)
		index = FindOldestNote();

	Note& note = noteBuffer[index];
	float freq = MidiNoteToFrequency(midiNote);
	InitNote(note, freq, type, env);

	Serial.print("StartNote: "); Serial.println(micros() - start);
	
#if 0
	int8_t modNoteOffset = ((24 * analogRead(A3)) >> 10) - 12;
	float modAmount = 8 * analogRead(A2) / 1023.0;
	float modAttack = analogRead(A1) / 512.0;

	Serial.print("modNoteOffset: "); Serial.println(modNoteOffset);
	Serial.print("modAmount: "); Serial.println(modAmount);
	Serial.print("modAttack: "); Serial.println(modAttack);
	
	note.modamount = modamount;
	note.mod = makemod(midinote + modnoteoffset, modattack);
#endif
}

float MidiNoteToFrequency(int8_t midiNote)
{
	int octave = midiNote / 12;
	int pitchIndex = midiNote % 12;
	
	float freq = freqs[pitchIndex];
	for (int i = 0; i < octave; ++i)
		freq = freq + freq;
	return freq;
}

void InitNote(Note& note, float freq, byte type, const Envelope& env)
{
	note.midiNote = midiNote;
	note.phase = 0;
	note.phaseDelta = freq * freqToDeltaScale;
	note.releaseDelta = 0;
	note.level = 0;
	note.release = env.release;
	note.stage = 0;
	note.type = type;
	note.ticks = 0;
	note.sustainLevel = float(ULONG_MAX) * env.sustain;
	note.attackDelta = env.attack ? float(ULONG_MAX) / (env.attack * sampleRate) : 0;
	note.decayDelta = env.decay ? (ULONG_MAX - note.sustainLevel) / (env.decay * sampleRate) : 0;
	note.mod = nullptr;
	note.modAmount = 0;
}

void StopNote(byte midiNote)
{
	int index = FindNote(midiNote);
	if (index >= 0)
	{
		Note& note = noteBuffer[index];
		if (note.release)
		{
			note.stage = 3;
			note.releaseDelta = note.release ? note.level / (note.release * sampleRate) : 0;
		}
		else
		{
			note.midiNote = -1;
			delete note.mod;
		}
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
		}
	}	
	
	SET_OUTPUT(0x800 + (output >> (16 + noteBufferBits)));
}
