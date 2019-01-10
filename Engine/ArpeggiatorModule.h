#pragma once 
#include "Module.h"

namespace Engine
{
class ArpeggiatorModule
{
public:
	ArpeggiatorModule(int polyphony, Array<MultiOutput>& outputs);
	void Update();
	void Connect(Array<MultiOutput>& midiOutputs);
	void SetHold(bool hold);
	void SetPeriod(uint16_t period);
	void SetOctaves(uint16_t octaves);

private:
	Array<MultiInput> _midiInputs;
	Array<MultiOutput>& _outputs;

	Array<float> _pitches;
	const int _polyphony;
	int _noteCount = 0;
	int _pressedCount = 0;
	int _currentPitch = 0;
	int _currentOctave = 0;
	int _currentOutput = -1;
	uint16_t _phase = 0;
	uint16_t _period = 0;
	uint16_t _octaves = 0;
	bool _waiting = true;
	bool _hold = false;
	bool _forceUpdate = false;
};
}