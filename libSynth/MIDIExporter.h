#pragma once

#include "Exporter.h"

namespace Synth
{
	class MIDIExporter : public Model::Exporter
	{
	public:
		BufferPtr Export(const std::wstring& path);
		BufferPtr ExportStopMIDI();
		BufferPtr ExportSetAllNotesOn();
		BufferPtr ExportNote(byte note, bool on);
	};
}
