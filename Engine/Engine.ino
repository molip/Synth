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

	auto floatToBytes = [](float val)
	{
		union { float f; uint32_t i; } u;

		u.f = val;
		return u.i;
	};

	auto getByte = [](uint32_t val, int byte)
	{
		return static_cast<uint8_t>(val >> byte * 8 & 0xff);
	};

	uint32_t attack = floatToBytes(100);
	uint32_t decay = floatToBytes(1000);
	uint32_t sustain = floatToBytes(0.5f);
	uint32_t release = floatToBytes(1000);

	byte inputData[] = 
	{
		(byte)CommandType::StartGraph,
		(byte)CommandType::InitGraph, 7, 2, 16, 
		(byte)CommandType::AddMonoModule, (byte)ModuleType::MIDI, // 0			
		(byte)CommandType::AddPolyModule, (byte)ModuleType::Envelope,		// 0
		(byte)CommandType::AddPolyModule, (byte)ModuleType::Oscillator,		// 1
		(byte)CommandType::AddMonoModule, (byte)ModuleType::PolyMixer,	// 1
		(byte)CommandType::AddMonoModule, (byte)ModuleType::Target,	// 2

		(byte)CommandType::SetValue, true, 0, Pin::Envelope::Input::Attack, getByte(attack, 3), getByte(attack, 2), getByte(attack, 1), getByte(attack, 0),
		(byte)CommandType::SetValue, true, 0, Pin::Envelope::Input::Decay, getByte(decay, 3), getByte(decay, 2), getByte(decay, 1), getByte(decay, 0),
		(byte)CommandType::SetValue, true, 0, Pin::Envelope::Input::Sustain, getByte(sustain, 3), getByte(sustain, 2), getByte(sustain, 1), getByte(sustain, 0),
		(byte)CommandType::SetValue, true, 0, Pin::Envelope::Input::Release, getByte(release, 3), getByte(release, 2), getByte(release, 1), getByte(release, 0),

		(byte)CommandType::AddConnection, (byte)ConnectionType::Single, 
			(byte)InstanceType::Poly, 1, Pin::Oscillator::Input::Level,
			(byte)InstanceType::Poly, 0, Pin::Envelope::Output::Level,
		(byte)CommandType::AddConnection, (byte)ConnectionType::Multi, 
			(byte)InstanceType::Poly, 0, Pin::Envelope::Input::Gate,
			(byte)InstanceType::Mono, 0, Pin::MIDI::MultiOutput::Gate,
		(byte)CommandType::AddConnection, (byte)ConnectionType::Multi, 
			(byte)InstanceType::Poly, 1, Pin::Oscillator::Input::Pitch,
			(byte)InstanceType::Mono, 0, Pin::MIDI::MultiOutput::Pitch,

		(byte)CommandType::AddConnection, (byte)ConnectionType::Multi, 
			(byte)InstanceType::Mono, 1, Pin::PolyMixer::MultiInput::Signal,
			(byte)InstanceType::Poly, 1, Pin::Oscillator::Output::Signal,
		(byte)CommandType::AddConnection,  (byte)ConnectionType::Single,  
			(byte)InstanceType::Mono, 2, Pin::Target::Input::Signal,
			(byte)InstanceType::Mono, 1, Pin::PolyMixer::Output::Signal,

		(byte)CommandType::EndGraph,
	};

	for (size_t i = 0; i < sizeof inputData; ++i)
		RemoteInput::Process(inputData[i]);
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
		RemoteInput::Process(data);
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
