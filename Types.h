struct Note
{
	int8_t midiNote;		// -1: null note.
	uint16_t phase; 		// [0, 0xffff]
	uint16_t phaseDelta;
	uint32_t level;
	float release; 			// s
	uint32_t attackDelta;
	uint32_t decayDelta;
	uint32_t releaseDelta;
	uint32_t sustainLevel;
	uint32_t ticks;
	byte stage;				// [0, 3]
	byte type;
	Note* mod;
	float modAmount; // [0, 1]
};

struct Envelope
{
	float attack, decay, release; // s
	float sustain; // [0, 1]
};