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
