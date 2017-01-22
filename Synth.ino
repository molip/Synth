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
#include "MixerModule.h"
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

class Instance
{
public:
	Instance() : attackValMod(100), decayValMod(1000), sustainValMod(0x8000), releaseValMod(1000)
	{
		envMod._attackInput.Connect(attackValMod._output);
		envMod._decayInput.Connect(decayValMod._output);
		envMod._sustainInput.Connect(sustainValMod._output);
		envMod._releaseInput.Connect(releaseValMod._output);
	
		oscMod._levelInput.Connect(envMod._output);
	}
	
	void Update()
	{
		envMod.Update();
		oscMod.Update();
	}
	
	UnsignedValueModule attackValMod, decayValMod, sustainValMod, releaseValMod;

	EnvelopeModule envMod;
	OscillatorModule oscMod;
};

Instance instances[MIDIModule::Polyphony];
MIDIModule midiMod;
MixerModule mixerMod(MIDIModule::Polyphony);
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

	for (int i = 0; i < MIDIModule::Polyphony; ++i)
	{
		instances[i].envMod._gateInput.Connect(midiMod._notes[i].gateOutput);
		instances[i].oscMod._pitchInput.Connect(midiMod._notes[i].pitchOutput);
		mixerMod._inputs[i].Connect(instances[i].oscMod._output);
	}

	outMod._input.Connect(mixerMod._ouput);
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
	for (int i = 0; i < MIDIModule::Polyphony; ++i)
		instances[i].Update();

	mixerMod.Update();
	outMod.Update();

	midiMod.ResetChanged();
}
