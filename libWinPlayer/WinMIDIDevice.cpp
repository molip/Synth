#include "stdafx.h"
#include "WinMIDIDevice.h"

#include "../libSynth/Controller.h"

#include "../libKernel/Debug.h"
#include "../libKernel/FormatString.h"
#include "../libKernel/Util.h"

#include "teVirtualMIDI/teVirtualMIDI.h"

namespace
{
	const int MAX_SYSEX_BUFFER = 65535;
}

WinMIDIDevice::WinMIDIDevice()
{
	//::virtualMIDILogging(TE_VM_LOGGING_MISC | TE_VM_LOGGING_RX | TE_VM_LOGGING_TX);

	auto callback = [](LPVM_MIDI_PORT midiPort, LPBYTE midiDataBytes, DWORD length, DWORD_PTR dwCallbackInstance)
	{
		reinterpret_cast<WinMIDIDevice*>(dwCallbackInstance)->ProcessMIDI(Synth::Buffer(midiDataBytes, midiDataBytes + length));
	};

	for (int i = 0; i < 16 && !_port; ++i)
	{
		const std::wstring name = Kernel::StringToWString(Kernel::FormatString("My Synth %0", i));
		_port = ::virtualMIDICreatePortEx2(name.c_str(), callback, reinterpret_cast<DWORD_PTR>(this), MAX_SYSEX_BUFFER, TE_VM_FLAGS_PARSE_RX | TE_VM_FLAGS_INSTANTIATE_RX_ONLY);
	}
}

WinMIDIDevice::~WinMIDIDevice()
{
	::virtualMIDIClosePort(_port);
}

void WinMIDIDevice::ProcessMIDI(Synth::Buffer&& buffer)
{
	if (m_controller)
		m_controller->ExportRawMIDI(std::move(buffer));
}
