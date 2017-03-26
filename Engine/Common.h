#pragma once

enum class ModuleType { None, MIDI, Envelope, Oscillator, PolyMixer, Mixer, Target, Filter, Range, PitchShift, _Count };

namespace Pin
{
	namespace MIDI
	{
		namespace UnsignedMultiOutput { enum Type { Gate, Pitch, _Count }; }
	}
	namespace Envelope 
	{
		namespace UnsignedInput { enum Type { Gate, Attack, Decay, Sustain, Release, _Count }; }
		namespace UnsignedOutput { enum Type { Level, _Count }; }
	}
	namespace Oscillator 
	{
		namespace UnsignedInput { enum Type { Waveform, Pitch, Level, _Count }; }
		namespace SignedInput { enum Type { PhaseMod, _Count }; }
		namespace SignedOutput { enum Type { Signal, _Count }; }
	}
	namespace PolyMixer 
	{
		namespace SignedMultiInput { enum Type { Signal, _Count }; }
		namespace SignedOutput { enum Type { Signal, _Count }; }
	}
	namespace Target
	{
		namespace SignedInput { enum Type { Signal, _Count }; }
	}
	namespace Filter
	{
		namespace UnsignedInput { enum Type { Frequency, Resonance, Bypass, _Count }; }
		namespace SignedInput { enum Type { Signal, _Count }; }
		namespace SignedOutput { enum Type { Signal, _Count }; }
	}
	namespace Range
	{
		namespace UnsignedInput { enum Type { Value, Minimum, Maximum, _Count }; }
		namespace UnsignedOutput { enum Type { Value, _Count }; }
	}
	namespace Mixer
	{
		namespace SignedInput { enum Type { Signal0, Signal1, Signal2, Signal3, _Count }; }
		namespace SignedOutput { enum Type { Signal, _Count }; }
	}
	namespace PitchShift
	{
		namespace SignedInput { enum Type { Shift, _Count }; }
		namespace UnsignedInput { enum Type { Pitch, _Count }; }
		namespace UnsignedOutput { enum Type { Pitch, _Count }; }
	}}

enum class CommandType { StartGraph, InitGraph, AddMonoModule, AddPolyModule, AddConnection, EndGraph, SetUnsignedValue, SetSignedValue, SetMIDIData };
enum class Error { None, UnknownCommandType, InvalidParameter, TooManyParameters, PinTypeMismatch, GraphAlreadyStarted };

enum class ConnectionType { None, Single, Multi, _Count };
enum class InstanceType { None, Mono, Poly, _Count };
enum class PinType { None, Signed, Unsigned, _Count };

