#include "stdafx.h"
#include "MIDIExporter.h"
#include "Model/Graph.h"
#include "Model/Engine.h"
#include "Model/Module.h"
#include "Model/ModuleTypes.h"

#include <fstream>

using namespace Synth;

BufferPtr MIDIExporter::Export(const std::wstring& path)
{
	std::ifstream file;
	file.open(path, std::ios::binary | std::ios::ate);
	if (!file.is_open())
		throw Exception("File open failed");

	size_t size = file.tellg();

	if (size < 14)
		throw Exception("Invalid MIDI file; too short.");

	char tag[4];
	long headerLength;
	short format, trackCount, division;

	auto ReadNumber = [&](void* dest, int bytes)
	{
		for (int i = bytes - 1; i >= 0; --i)
			file.read((char*)dest + i, 1);
	};

	file.seekg(0, std::ios::beg);
	file.read(tag, 4);
	ReadNumber(&headerLength, 4);
	ReadNumber(&format, 2);
	ReadNumber(&trackCount, 2);
	ReadNumber(&division, 2);

	if (::strncmp(tag, "MThd", 4) || headerLength != 6)
		throw Exception("Invalid MIDI file; header not found.");

	if (format != 0)
		throw Exception("Unsupported MIDI file; must be type 0.");

	if (trackCount != 1)
		throw Exception("Unsupported MIDI file; must contain single track.");

	file.read(tag, 4);
	if (::strncmp(tag, "MTrk", 4))
		throw Exception("Invalid MIDI file; track header not found.");

	long trackLength;
	ReadNumber(&trackLength, 4);

	if ((long)file.tellg() + trackLength > size)
		throw Exception("Invalid MIDI file; track data truncated.");

	Add((byte)Engine::CommandType::SetMIDIData);
	AddInteger(division, 2);
	AddInteger(trackLength, 4);

	const size_t trackDataStart = _buffer->size();
	_buffer->resize(trackDataStart + trackLength);
	file.read((char*)&(*_buffer)[trackDataStart], trackLength);

	return std::move(_buffer);
}

BufferPtr MIDIExporter::ExportStopMIDI()
{
	Add((byte)Engine::CommandType::StopMIDIPlayback);
	return std::move(_buffer);
}

BufferPtr MIDIExporter::ExportSetAllNotesOn()
{
	Add((byte)Engine::CommandType::SetAllNotesOn);
	return std::move(_buffer);
}

BufferPtr MIDIExporter::ExportNote(byte note, bool on)
{
	_buffer = std::make_unique<Buffer>();

	Add((byte)Engine::CommandType::MIDIInput);
	AddInteger(3, 4);		// 3 bytes.
	Add(on ? 144 : 128);	// Note on.
	Add(note);				// Pitch.
	Add(on ? 100 : 0);		// Velocity.

	return std::move(_buffer);
}

BufferPtr MIDIExporter::ExportRawMIDI(Buffer&& buffer)
{
	_buffer = std::make_unique<Buffer>();

	Add((byte)Engine::CommandType::MIDIInput);
	AddInteger((int)buffer.size(), 4);
	_buffer->insert(_buffer->end(), buffer.begin(), buffer.end());

	return std::move(_buffer);
}
