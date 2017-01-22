#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <limits.h>

#include <avr/pgmspace.h>

#include "Types.h"
#include "Sine.h"
#include "Config.h"

#include "EnvelopeModule.h"
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

UnsignedValueModule attackValMod(100), decayValMod(1000), sustainValMod(0x8000), releaseValMod(1000);

MIDIModule midiMod;
EnvelopeModule envMod;
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

	envMod._attackInput._source = &attackValMod._output;
	envMod._decayInput._source = &decayValMod._output;
	envMod._sustainInput._source = &sustainValMod._output;
	envMod._releaseInput._source = &releaseValMod._output;
	envMod._gateInput._source = &midiMod._gateOutput;
	
	oscMod._levelInput._source = &envMod._output;
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
	envMod.Update();
	oscMod.Update();
	outMod.Update();

	midiMod._gateOutput.ResetChanged();
	midiMod._pitchOutput.ResetChanged();
	envMod._output.ResetChanged();
}
