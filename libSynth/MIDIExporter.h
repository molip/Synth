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
	private:
		void Add(byte data) { (*_buffer).push_back(data); }

		BufferPtr _buffer;
	};
}
