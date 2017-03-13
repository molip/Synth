#pragma once

#include <arduino.h>
//using byte = unsigned char;
//using uint32_t = unsigned int;

class MIDIData
{
public:
	MIDIData();
	~MIDIData();

	static MIDIData& Instance();

	void ResetData();
	void SetData(const byte* data, uint32_t size, int16_t division);
	bool HasData() const { return _data != nullptr; }

	void DoTick();
	bool GetData(byte& data);

private:
	void SetTempo(uint32_t tempo);
	void UpdateNextEvent();
	bool IsFinished() const;
	uint32_t ReadVariableData();
	byte ReadByte();

	const byte* _data = nullptr;
	const byte* _current = nullptr;
	int16_t _division = 0;
	uint32_t _size = 0;
	uint32_t _ticksTillNextEvent = 0;
	uint32_t _ticksPerMidiTick = 0;
	size_t _commandBytesLeft = 0;
};