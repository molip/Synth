#pragma once

namespace Pin
{
	namespace MIDI
	{
		namespace UnsignedPolyOutput { enum Type { Gate, Pitch, _Count }; }
	}
	namespace Envelope 
	{
		namespace UnsignedInput { enum Type { Gate, Attack, Decay, Sustain, Release, _Count }; }
		namespace UnsignedOutput { enum Type { Level, _Count }; }
	}
	namespace Oscillator 
	{
		namespace UnsignedInput { enum Type { Pitch, Level, _Count }; }
		namespace SignedOutput { enum Type { Signal, _Count }; }
	}
	namespace PolyMixer 
	{
		namespace SignedPolyInput { enum Type { Signal, _Count }; }
		namespace SignedOutput { enum Type { Signal, _Count }; }
	}
	namespace Target
	{
		namespace SignedInput { enum Type { Signal, _Count }; }
	}
}
