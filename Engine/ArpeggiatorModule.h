#pragma once 
#include "Module.h"

namespace Engine
{
class ArpeggiatorModule : public Module
{
public:
	ArpeggiatorModule(int polyphony);
	virtual void Update() override;
	virtual bool WantUpdate() const override { return true; }

private:
	Array<unsigned_t> _pitches;
	const int _polyphony;
	int _noteCount = 0;
	int _currentPitch = 0;
	int _currentOctave = 0;
	int _currentOutput = -1;
	uint16_t _phase = 0;
	uint16_t _period = 0;
	uint16_t _octaves = 0;
	bool _waiting = true;
};
}