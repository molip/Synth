#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <limits.h>
#include <ADC.h>

#include <avr/pgmspace.h>

#include "Types.h"
#include "Sine.h"
#include "Config.h"
#include "Graph.h"
#include "Input.h"
#include "MIDIData.h"

using namespace Engine;

#define MIDISERIAL Serial2

boolean _gotTick;

uint32_t _lastReport;
uint8_t _currentKnob;

ADC* _adc = new ADC();

void setup()
{
	Config::Init();

	pinMode(4, INPUT_PULLUP);
	pinMode(5, INPUT_PULLUP);
	pinMode(6, INPUT_PULLUP);
	pinMode(7, INPUT_PULLUP);
	
	analogWriteResolution(12);

	_adc->setAveraging(16);
	_adc->startSingleRead(0);

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

	if (_gotTick)
	{
		midiData.DoTick();

		_gotTick = false;
		unsigned long start = micros();
		DoTick();
		
		if (start > _lastReport + 1000000)
		{	
			_lastReport = start;
			Serial.println(micros() - start);
		}
	}
}

void timer()
{
	_gotTick = true;
}

void DoTick()
{
	int16_t adcVal = -1;
	if (_adc->isComplete())
	{
		adcVal = _adc->readSingle();
		_adc->startSingleRead(0);
	}

	if (Graph* graph = Graph::GetActive())
	{
		if (adcVal >= 0)
		{
			if (const int knobCount = graph->GetKnobCount())
			{
				_currentKnob = _currentKnob % knobCount;
				graph->UpdateKnob(_currentKnob++, adcVal);
			}
		}

		graph->Update();
	}
}
