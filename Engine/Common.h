#pragma once

enum class ModuleType { None, MIDI, Envelope, Oscillator, Mixer, Target, UnsignedValue, _Count };

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

enum class CommandType { StartGraph, InitGraph, AddMonoModule, AddPolyModule, AddConnection, EndGraph, SetMonoUnsignedValue, SetPolyUnsignedValue };
enum class Error { None, UnknownCommandType, InvalidParameter, TooManyParameters, PinTypeMismatch, GraphAlreadyStarted };

enum class InstanceType { None, Mono, Poly, _Count };
enum class PinType { None, Signed, Unsigned, _Count };

