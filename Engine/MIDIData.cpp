#include "MIDIData.h"
#include "Config.h"

MIDIData::MIDIData()
{
}

MIDIData& MIDIData::Instance()
{
	static MIDIData instance;
	return instance;
}

void MIDIData::ResetData()
{
	delete[] _data;
	_current = _data = nullptr;
	_size = 0;
	_commandBytesLeft = 0;
}

void MIDIData::SetData(const byte* data, uint32_t size, int16_t division) // division = midi ticks per beat.
{
	ResetData();
	_current = _data = data;
	_size = size;
	_division = division;

	SetTempo(500000); // 120bpm.

	UpdateNextEvent();
}

MIDIData::~MIDIData()
{
	delete[] _data;
}

void MIDIData::SetTempo(uint32_t tempo) // microseconds per beat.
{
	_ticksPerMidiTick = Config::sampleRateMS * tempo / (_division * 1000); 
}

void MIDIData::DoTick()
{
	if (_ticksTillNextEvent)
		--_ticksTillNextEvent;
}

bool MIDIData::GetData(byte& data)
{
	if (_commandBytesLeft) // Continue command.
	{
		data = ReadByte();

		if (--_commandBytesLeft == 0) // Finished. 
			UpdateNextEvent();

		return true;
	}

	if (_ticksTillNextEvent)
		return false;

	byte newData = ReadByte();

	// TODO: process one byte per call.
	if (newData == 0xff) // Meta event. 
	{
		byte type = *_current++;
		uint32_t bytes = ReadVariableData();
		
		if (type == 0x51) // Tempo.
		{
			uint32_t tempo = 0;
			for (size_t i = 0; i < bytes; ++i)
				tempo = (tempo << 8) | *_current++;

			SetTempo(tempo);
		}
		else
		{
			_current += bytes;
		}
		UpdateNextEvent();
		return false;
	}
	
	if (newData == 0xf0 || newData == 0xf7) // SysEx event. 
	{
		while (ReadByte() != 0xf7); // Skip.
		UpdateNextEvent();
		return false;
	}
		
	// Track Event - start command. 
	_commandBytesLeft = newData >= 192 && newData < 224 ? 1 : 2; 
	data = newData;

	if ((data & 0x80) == 0) // Running status.
		--_commandBytesLeft;

	return true;
}

byte MIDIData::ReadByte()
{
	if (IsFinished())
	{
		Serial.println("[MIDIData::ReadVariableData] Out of data!");
		return 0;
	}
	return *_current++;
}

uint32_t MIDIData::ReadVariableData()
{
	uint32_t val = 0;
	while (true)
	{
		byte data = ReadByte();
		val = (val << 7) | (data & 0x7f);
		if ((data & 0x80) == 0)
			break;
	}
	return val;
}

void MIDIData::UpdateNextEvent()
{
	if (IsFinished())
	{
		Serial.println("Finished");
		_current = _data;
		UpdateNextEvent();
	}
	else
	{
		uint32_t delta = ReadVariableData();
		_ticksTillNextEvent = delta * _ticksPerMidiTick;
	}
}

bool MIDIData::IsFinished() const
{
	return _current >= _data + _size;
}
