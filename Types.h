struct Note
{
	int8_t midiNote;		// -1: null note.
	uint16_t phase; 		// [0, 0xffff]
	uint16_t delta;
	unsigned int ticksLeft;
	byte type;
};
