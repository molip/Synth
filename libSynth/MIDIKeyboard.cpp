#include "stdafx.h"
#include "MIDIKeyboard.h"
#include "MIDIExporter.h"

#include "../libKernel/Debug.h"

#include <string>

using namespace Synth;

namespace
{
	const byte RootKey = 60;
}

MIDIKeyboard::MIDIKeyboard(const KeyCodes& keyCodes) : _keyCodes(keyCodes)
{
	_keyStates.resize(_keyCodes.size());
}

BufferPtr MIDIKeyboard::OnKeyStateChanged(KeyCode key, bool state)
{
	auto it = std::find(_keyCodes.begin(), _keyCodes.end(), key);
	if (it == _keyCodes.end())
		return {};

	const size_t index = it - _keyCodes.begin();
		
	if (_keyStates[index] == state)
		return {};

	_keyStates[index] = state;
	return MIDIExporter().ExportNote(::RootKey + static_cast<byte>(index), state);
}
