#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <limits.h>

#include <avr/pgmspace.h>

#include "Types.h"
#include "Sine.h"
#include "Config.h"

#include "MIDIModule.h"
#include "OscillatorModule.h"
#include "OutputModule.h"

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

boolean gotTick;

uint32_t lastReport;
 
MIDIModule midiMod;
OscillatorModule oscMod;
OutputModule outMod;

void setup()
{
	Config::Init();

	pinMode(4, INPUT_PULLUP);
	pinMode(5, INPUT_PULLUP);
	pinMode(6, INPUT_PULLUP);
	pinMode(7, INPUT_PULLUP);
	
	INIT_DAC();

	const unsigned long interval = 1000000 / Config::sampleRate;
	Timer1.initialize(interval); 
	Timer1.attachInterrupt(timer);

	Serial.begin(57600);

	MIDISERIAL.begin(31250);

	oscMod._levelInput._source = &midiMod._gateOutput;
	oscMod._pitchInput._source = &midiMod._pitchOutput;
	outMod._input._source = &oscMod._output;
}

void loop()
{
	if (MIDISERIAL.available() > 0) 
	{
		byte midiByte = MIDISERIAL.read();
		midiMod.ProcessMIDI(midiByte);
	}

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
}

void timer()
{
	gotTick = true;
}

void DoTick()
{
	oscMod.Update();
	outMod.Update();

	midiMod._gateOutput.ResetChanged();
	midiMod._pitchOutput.ResetChanged();
}
