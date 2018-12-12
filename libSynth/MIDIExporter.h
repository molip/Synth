#pragma once

#include "Model/Engine.h"
#include "Types.h"

namespace Synth
{
	class Graph;
	class MIDIExporter
	{
	public:
		BufferPtr Export(const std::wstring& path);
		BufferPtr ExportStopMIDI();
		BufferPtr ExportSetAllNotesOn();
		BufferPtr ExportNote(byte note, bool on);
	private:
		void Add(byte data) { (*_buffer).push_back(data); }
		void AddNumber(uint32_t val, int bytes);

		BufferPtr _buffer;
	};
}
