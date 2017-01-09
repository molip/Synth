struct Note
{
	uint16_t phase; 		// [0, 0xffff]
	uint16_t delta;
	unsigned int ticksLeft;
	byte type;
};
