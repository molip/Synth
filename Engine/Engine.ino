#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <limits.h>

#include <avr/pgmspace.h>

#include "Types.h"
#include "Sine.h"
#include "Config.h"
#include "Graph.h"
#include "Input.h"

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

void TestInput()
{
	delay(1000);

	Serial.println("Hello");

	byte inputData[] = 
	{
		(byte)CommandType::StartGraph,
		(byte)CommandType::InitGraph, 7, 2, 16, 
		(byte)CommandType::AddMonoModule, (byte)ModuleType::MIDI, // 0			
		(byte)CommandType::AddPolyModule, (byte)ModuleType::Envelope,		// 0
		(byte)CommandType::AddPolyModule, (byte)ModuleType::Oscillator,		// 1
		(byte)CommandType::AddMonoModule, (byte)ModuleType::Mixer,	// 1
		(byte)CommandType::AddMonoModule, (byte)ModuleType::Target,	// 2
		(byte)CommandType::AddMonoModule, (byte)ModuleType::UnsignedValue,	// 3 (attack)
		(byte)CommandType::AddMonoModule, (byte)ModuleType::UnsignedValue,	// 4 (decay)
		(byte)CommandType::AddMonoModule, (byte)ModuleType::UnsignedValue,	// 5 (sustain)
		(byte)CommandType::AddMonoModule, (byte)ModuleType::UnsignedValue,	// 6 (release)

		(byte)CommandType::SetMonoUnsignedValue, 3, 100 & 0xff, 100 >> 8, 
		(byte)CommandType::SetMonoUnsignedValue, 4, 1000 & 0xff, 1000 >> 8, 
		(byte)CommandType::SetMonoUnsignedValue, 5, 0x8000 & 0xff, 0x8000 >> 8, 
		(byte)CommandType::SetMonoUnsignedValue, 6, 1000 & 0xff, 1000 >> 8, 

		(byte)CommandType::AddConnection, (byte)ConnectionType::Single, 
			(byte)InstanceType::Poly, (byte)PinType::Unsigned, 0, Pin::Envelope::UnsignedInput::Attack,
			(byte)InstanceType::Mono, (byte)PinType::Unsigned, 3, 0,
		(byte)CommandType::AddConnection, (byte)ConnectionType::Single, 
			(byte)InstanceType::Poly, (byte)PinType::Unsigned, 0, Pin::Envelope::UnsignedInput::Decay,
			(byte)InstanceType::Mono, (byte)PinType::Unsigned, 4, 0,
		(byte)CommandType::AddConnection, (byte)ConnectionType::Single, 
			(byte)InstanceType::Poly, (byte)PinType::Unsigned, 0, Pin::Envelope::UnsignedInput::Sustain,
			(byte)InstanceType::Mono, (byte)PinType::Unsigned, 5, 0,
		(byte)CommandType::AddConnection, (byte)ConnectionType::Single, 
			(byte)InstanceType::Poly, (byte)PinType::Unsigned, 0, Pin::Envelope::UnsignedInput::Release,
			(byte)InstanceType::Mono, (byte)PinType::Unsigned, 6, 0,

		(byte)CommandType::AddConnection, (byte)ConnectionType::Single, 
			(byte)InstanceType::Poly, (byte)PinType::Unsigned, 1, Pin::Oscillator::UnsignedInput::Level,
			(byte)InstanceType::Poly, (byte)PinType::Unsigned, 0, Pin::Envelope::UnsignedOutput::Level,
		(byte)CommandType::AddConnection, (byte)ConnectionType::Multi, 
			(byte)InstanceType::Poly, (byte)PinType::Unsigned, 0, Pin::Envelope::UnsignedInput::Gate,
			(byte)InstanceType::Mono, (byte)PinType::Unsigned, 0, Pin::MIDI::UnsignedMultiOutput::Gate,
		(byte)CommandType::AddConnection, (byte)ConnectionType::Multi, 
			(byte)InstanceType::Poly, (byte)PinType::Unsigned, 1, Pin::Oscillator::UnsignedInput::Pitch,
			(byte)InstanceType::Mono, (byte)PinType::Unsigned, 0, Pin::MIDI::UnsignedMultiOutput::Pitch,

		(byte)CommandType::AddConnection, (byte)ConnectionType::Multi, 
			(byte)InstanceType::Mono, (byte)PinType::Signed, 1, Pin::PolyMixer::SignedMultiInput::Signal,
			(byte)InstanceType::Poly, (byte)PinType::Signed, 1, Pin::Oscillator::SignedOutput::Signal,
		(byte)CommandType::AddConnection,  (byte)ConnectionType::Single,  
			(byte)InstanceType::Mono, (byte)PinType::Signed, 2, Pin::Target::SignedInput::Signal,
			(byte)InstanceType::Mono, (byte)PinType::Signed, 1, Pin::PolyMixer::SignedOutput::Signal,

		(byte)CommandType::EndGraph,
	};

	for (size_t i = 0; i < sizeof inputData; ++i)
		Input::Process(inputData[i]);
}

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

	//TestInput();
}


void loop()
{
    Graph* graph = Graph::GetActive();

    if (Serial.available() > 0) 
    {
        byte data = Serial.read();
        Input::Process(data);
    }

	if (MIDISERIAL.available() > 0) 
	{
        byte data = MIDISERIAL.read();
        if (graph)
            graph->ProcessMIDI(data);
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
	if (Graph* graph = Graph::GetActive())
		graph->Update();
}
