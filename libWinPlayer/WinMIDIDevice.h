#pragma once

#include "../libSynth/Types.h"

namespace Synth::UI
{
	class Controller;
}

struct _VM_MIDI_PORT;

class WinMIDIDevice 
{
public:
	WinMIDIDevice();
	~WinMIDIDevice();

	void SetController(Synth::UI::Controller* controller) { m_controller = controller; }

private:
	void ProcessMIDI(Synth::Buffer&& buffer);

	Synth::UI::Controller* m_controller{};
	_VM_MIDI_PORT* _port{};
};
