#pragma once

#include "Types.h"

namespace Synth
{
	class MIDIKeyboard 
	{
	public:
		using KeyCode = uint8_t;
		using KeyCodes = std::vector<KeyCode>;

		MIDIKeyboard(const KeyCodes& keyCodes);

		BufferPtr OnKeyStateChanged(KeyCode key, bool state);

	private:
		const KeyCodes _keyCodes;
		std::vector<bool> _keyStates;
	};
}
