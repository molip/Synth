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
#include "PolyMixerModule.h"
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
	Instance()
	{
		attackValMod.SetValue(100);
		decayValMod.SetValue(1000);
		sustainValMod.SetValue(0x8000);
		releaseValMod.SetValue(1000);

		envMod.GetPins<UnsignedInput>()[Pin::Envelope::UnsignedInput::Attack].Connect(attackValMod.GetPins<UnsignedOutput>()[0]);
		envMod.GetPins<UnsignedInput>()[Pin::Envelope::UnsignedInput::Decay].Connect(decayValMod.GetPins<UnsignedOutput>()[0]);
		envMod.GetPins<UnsignedInput>()[Pin::Envelope::UnsignedInput::Sustain].Connect(sustainValMod.GetPins<UnsignedOutput>()[0]);
		envMod.GetPins<UnsignedInput>()[Pin::Envelope::UnsignedInput::Release].Connect(releaseValMod.GetPins<UnsignedOutput>()[0]);
	
		oscMod.GetPins<UnsignedInput>()[Pin::Oscillator::UnsignedInput::Level].Connect(envMod.GetPins<UnsignedOutput>()[Pin::Envelope::UnsignedOutput::Level]);
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

const int Polyphony = 16;

Instance instances[Polyphony];
MIDIModule midiMod(Polyphony);
PolyMixerModule mixerMod;
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

	midiMod.SetPolyphony(Polyphony);
	mixerMod.SetPolyphony(Polyphony);

	for (int i = 0; i < Polyphony; ++i)
	{
		instances[i].envMod.GetPins<UnsignedInput>()[Pin::Envelope::UnsignedInput::Gate].Connect(midiMod.GetPolyPins<UnsignedOutput>()[Pin::MIDI::UnsignedPolyOutput::Gate][i]);
		instances[i].oscMod.GetPins<UnsignedInput>()[Pin::Oscillator::UnsignedInput::Pitch].Connect(midiMod.GetPolyPins<UnsignedOutput>()[Pin::MIDI::UnsignedPolyOutput::Pitch][i]);
		
		mixerMod.GetPolyPins<SignedInput>()[Pin::PolyMixer::SignedPolyInput::Signal][i].Connect(instances[i].oscMod.GetPins<SignedOutput>()[Pin::Oscillator::SignedOutput::Signal]);
	}

	outMod.GetPins<SignedInput>()[Pin::Target::SignedInput::Signal].Connect(mixerMod.GetPins<SignedOutput>()[Pin::PolyMixer::SignedOutput::Signal]);
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
	for (int i = 0; i < Polyphony; ++i)
		instances[i].Update();

	mixerMod.Update();
	outMod.Update();

	midiMod.ResetChanged();
}
