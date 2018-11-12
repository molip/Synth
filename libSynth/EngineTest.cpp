#include "stdafx.h"
#include "EngineTest.h"

#include "../Engine/Common.h"
#include "../Engine/Input.h"

using namespace Synth;

EngineTest::EngineTest()
{
	Engine::Config::Init();
}

EngineTest& EngineTest::Instance()
{
	static EngineTest instance;
	return instance;
}

bool EngineTest::Test(const Buffer& buffer)
{
	for (byte b : buffer)
		Engine::Input::Process(b);

	if (Engine::Graph* graph = Engine::Graph::GetActive())
	{
		graph->ProcessMIDI(144); //Note on.
		graph->ProcessMIDI(64); //Note.
		graph->ProcessMIDI(100); //Velocity.
		
		for (int i = 0; i < 100; ++i)
			graph->Update();
	}
	return true;
}
