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

	using namespace Input;
	using ModType = AddConnectionCommand::Connection::ModType;
	using PinType = AddConnectionCommand::Connection::PinType;

	byte inputData[] = 
	{
		(byte)CommandType::StartGraph,
		(byte)CommandType::InitGraph, 3, 6, 16, 
		(byte)CommandType::AddMonoModule, (byte)ModuleType::MIDI, // 0			
		(byte)CommandType::AddPolyModule, (byte)ModuleType::UnsignedValue,	// 0 (attack)
		(byte)CommandType::AddPolyModule, (byte)ModuleType::UnsignedValue,	// 1 (decay)
		(byte)CommandType::AddPolyModule, (byte)ModuleType::UnsignedValue,	// 2 (sustain)
		(byte)CommandType::AddPolyModule, (byte)ModuleType::UnsignedValue,	// 3 (release)
		(byte)CommandType::AddPolyModule, (byte)ModuleType::Envelope,		// 4
		(byte)CommandType::AddPolyModule, (byte)ModuleType::Oscillator,		// 5
		(byte)CommandType::AddMonoModule, (byte)ModuleType::Mixer,	// 1
		(byte)CommandType::AddMonoModule, (byte)ModuleType::Target,	// 2

		(byte)CommandType::SetPolyUnsignedValue, 0, 100 & 0xff, 100 >> 8, 
		(byte)CommandType::SetPolyUnsignedValue, 1, 1000 & 0xff, 1000 >> 8, 
		(byte)CommandType::SetPolyUnsignedValue, 2, 0x8000 & 0xff, 0x8000 >> 8, 
		(byte)CommandType::SetPolyUnsignedValue, 3, 1000 & 0xff, 1000 >> 8, 

		(byte)CommandType::AddConnection, 
			(byte)ModType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedInput::Attack,
			(byte)ModType::Poly, (byte)PinType::Unsigned, 0, 0,
		(byte)CommandType::AddConnection, 
			(byte)ModType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedInput::Decay,
			(byte)ModType::Poly, (byte)PinType::Unsigned, 1, 0,
		(byte)CommandType::AddConnection, 
			(byte)ModType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedInput::Sustain,
			(byte)ModType::Poly, (byte)PinType::Unsigned, 2, 0,
		(byte)CommandType::AddConnection, 
			(byte)ModType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedInput::Release,
			(byte)ModType::Poly, (byte)PinType::Unsigned, 3, 0,

		(byte)CommandType::AddConnection, 
			(byte)ModType::Poly, (byte)PinType::Unsigned, 5, Pin::Oscillator::UnsignedInput::Level,
			(byte)ModType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedOutput::Level,
		(byte)CommandType::AddConnection, 
			(byte)ModType::Poly, (byte)PinType::Unsigned, 4, Pin::Envelope::UnsignedInput::Gate,
			(byte)ModType::Mono, (byte)PinType::Unsigned, 0, Pin::MIDI::UnsignedPolyOutput::Gate,
		(byte)CommandType::AddConnection, 
			(byte)ModType::Poly, (byte)PinType::Unsigned, 5, Pin::Oscillator::UnsignedInput::Pitch,
			(byte)ModType::Mono, (byte)PinType::Unsigned, 0, Pin::MIDI::UnsignedPolyOutput::Pitch,

		(byte)CommandType::AddConnection, 
			(byte)ModType::Mono, (byte)PinType::Signed, 1, Pin::PolyMixer::SignedPolyInput::Signal,
			(byte)ModType::Poly, (byte)PinType::Signed, 5, Pin::Oscillator::SignedOutput::Signal,
		(byte)CommandType::AddConnection, 
			(byte)ModType::Mono, (byte)PinType::Signed, 2, Pin::Target::SignedInput::Signal,
			(byte)ModType::Mono, (byte)PinType::Signed, 1, Pin::PolyMixer::SignedOutput::Signal,

		(byte)CommandType::EndGraph,
	};

	for (int i = 0; i < sizeof inputData; ++i)
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

	TestInput();
}


void loop()
{
    Graph* graph = Graph::GetActive();

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
