#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <limits.h>

#include <avr/pgmspace.h>

#include "Types.h"
#include "Sine.h"
#include "Config.h"
#include "Graph.h"
#include "Input.h"
#include "MIDIData.h"

using namespace Engine;

#define MIDISERIAL Serial2

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
		(byte)CommandType::AddMonoModule, (byte)ModuleType::PolyMixer,	// 1
		(byte)CommandType::AddMonoModule, (byte)ModuleType::Target,	// 2

		(byte)CommandType::SetUnsignedValue, true, 0, Pin::Envelope::SignedInput::Attack, 100 >> 8, 100 & 0xff, 
		(byte)CommandType::SetUnsignedValue, true, 0, Pin::Envelope::SignedInput::Decay, 1000 >> 8, 1000 & 0xff,
		(byte)CommandType::SetUnsignedValue, true, 0, Pin::Envelope::SignedInput::Sustain, 0x8000 >> 8, 0x8000 & 0xff,
		(byte)CommandType::SetUnsignedValue, true, 0, Pin::Envelope::SignedInput::Release, 1000 >> 8, 1000 & 0xff, 

		(byte)CommandType::AddConnection, (byte)ConnectionType::Single, 
			(byte)InstanceType::Poly, 1, Pin::Oscillator::SignedInput::Level,
			(byte)InstanceType::Poly, 0, Pin::Envelope::SignedOutput::Level,
		(byte)CommandType::AddConnection, (byte)ConnectionType::Multi, 
			(byte)InstanceType::Poly, 0, Pin::Envelope::SignedInput::Gate,
			(byte)InstanceType::Mono, 0, Pin::MIDI::SignedMultiOutput::Gate,
		(byte)CommandType::AddConnection, (byte)ConnectionType::Multi, 
			(byte)InstanceType::Poly, 1, Pin::Oscillator::SignedInput::Pitch,
			(byte)InstanceType::Mono, 0, Pin::MIDI::SignedMultiOutput::Pitch,

		(byte)CommandType::AddConnection, (byte)ConnectionType::Multi, 
			(byte)InstanceType::Mono, 1, Pin::PolyMixer::SignedMultiInput::Signal,
			(byte)InstanceType::Poly, 1, Pin::Oscillator::SignedOutput::Signal,
		(byte)CommandType::AddConnection,  (byte)ConnectionType::Single,  
			(byte)InstanceType::Mono, 2, Pin::Target::SignedInput::Signal,
			(byte)InstanceType::Mono, 1, Pin::PolyMixer::SignedOutput::Signal,

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
	
	analogWriteResolution(12);

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

	MIDIData& midiData = MIDIData::Instance();

	if (midiData.HasData()) 
	{
		// Wait until we have a graph so it can get the start.
		byte data = 0;
		if (graph && midiData.GetData(data))
			graph->ProcessMIDI(data);
	}
	else if (MIDISERIAL.available() > 0) 
	{
	    byte data = MIDISERIAL.read();
	    if (graph)
	        graph->ProcessMIDI(data);
	}

	if (gotTick)
	{
		midiData.DoTick();

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
