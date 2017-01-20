#pragma once 
#include "Module.h"
class MIDIModule : public Module
{
public:
	virtual void Update() {} // ?

	void ProcessMIDI(byte midiByte);

	UnsignedOutput _gateOutput, _pitchOutput;

private:
	byte _midiCommand = 0;
	byte _midiNote = 0;
};
