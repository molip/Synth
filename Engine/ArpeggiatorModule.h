#pragma once 
#include "Module.h"

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
	unsigned_t _phase = 0;
	unsigned_t _period = 0;
	unsigned_t _octaves = 0;
	bool _waiting = true;
};
