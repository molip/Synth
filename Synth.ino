#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <limits.h>

#include <avr/pgmspace.h>

#include "Types.h"
#include "Sine.h"
#include "Config.h"
#include "Graph.h"

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

	Graph* graph = new Graph;
	graph->Init(3, 6, 16);

	graph->AddMonoModule(ModuleType::MIDI);		// 0
	
	graph->AddPolyModule(ModuleType::UnsignedValue);	// 0 (attack)
	graph->AddPolyModule(ModuleType::UnsignedValue);	// 1 (decay)
	graph->AddPolyModule(ModuleType::UnsignedValue);	// 2 (sustain)
	graph->AddPolyModule(ModuleType::UnsignedValue);	// 3 (release)
	graph->AddPolyModule(ModuleType::Envelope);			// 4
	graph->AddPolyModule(ModuleType::Oscillator);		// 5

	graph->AddMonoModule(ModuleType::Mixer);	// 1
	graph->AddMonoModule(ModuleType::Target);	// 2

	for (int i = 0; i < graph->GetPolyphony(); ++i)
	{
		static_cast<UnsignedValueModule*>(graph->GetPolyModule(0, i))->SetValue(100);
		static_cast<UnsignedValueModule*>(graph->GetPolyModule(1, i))->SetValue(1000);
		static_cast<UnsignedValueModule*>(graph->GetPolyModule(2, i))->SetValue(0x8000);
		static_cast<UnsignedValueModule*>(graph->GetPolyModule(3, i))->SetValue(1000);

		graph->GetPolyModule(4, i)->GetPins<UnsignedInput>()[Pin::Envelope::UnsignedInput::Attack].Connect(graph->GetPolyModule(0, i)->GetPins<UnsignedOutput>()[0]);
		graph->GetPolyModule(4, i)->GetPins<UnsignedInput>()[Pin::Envelope::UnsignedInput::Decay].Connect(graph->GetPolyModule(1, i)->GetPins<UnsignedOutput>()[0]);
		graph->GetPolyModule(4, i)->GetPins<UnsignedInput>()[Pin::Envelope::UnsignedInput::Sustain].Connect(graph->GetPolyModule(2, i)->GetPins<UnsignedOutput>()[0]);
		graph->GetPolyModule(4, i)->GetPins<UnsignedInput>()[Pin::Envelope::UnsignedInput::Release].Connect(graph->GetPolyModule(3, i)->GetPins<UnsignedOutput>()[0]);
	
		graph->GetPolyModule(5, i)->GetPins<UnsignedInput>()[Pin::Oscillator::UnsignedInput::Level].Connect(graph->GetPolyModule(4, i)->GetPins<UnsignedOutput>()[Pin::Envelope::UnsignedOutput::Level]);

		graph->GetPolyModule(4, i)->GetPins<UnsignedInput>()[Pin::Envelope::UnsignedInput::Gate].Connect(graph->GetMonoModule(0)->GetPolyPins<UnsignedOutput>()[Pin::MIDI::UnsignedPolyOutput::Gate][i]);
		graph->GetPolyModule(5, i)->GetPins<UnsignedInput>()[Pin::Oscillator::UnsignedInput::Pitch].Connect(graph->GetMonoModule(0)->GetPolyPins<UnsignedOutput>()[Pin::MIDI::UnsignedPolyOutput::Pitch][i]);
		
		graph->GetMonoModule(1)->GetPolyPins<SignedInput>()[Pin::PolyMixer::SignedPolyInput::Signal][i].Connect(graph->GetPolyModule(5, i)->GetPins<SignedOutput>()[Pin::Oscillator::SignedOutput::Signal]);
	}

	graph->GetMonoModule(2)->GetPins<SignedInput>()[Pin::Target::SignedInput::Signal].Connect(graph->GetMonoModule(1)->GetPins<SignedOutput>()[Pin::PolyMixer::SignedOutput::Signal]);

	graph->Activate();
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
