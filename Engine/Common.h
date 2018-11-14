#pragma once

namespace Engine
{
enum class ModuleType { None, MIDI, Envelope, Oscillator, PolyMixer, Mixer, Target, Filter, Range, PitchShift, LFO, Delay, Arpeggiator, _Count };

namespace Pin
{
	namespace MIDI
	{
		namespace SignedMultiOutput { enum Type { Pitch, Gate, _Count }; }
	}
	namespace Envelope 
	{
		namespace SignedInput { enum Type { Gate, Sustain, Attack, Decay, Release, _Count }; }
		namespace SignedOutput { enum Type { Level, _Count }; }
	}
	namespace Oscillator 
	{
		namespace SignedInput { enum Type { Pitch, PhaseMod, Level, Waveform, _Count }; }
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
		namespace SignedInput { enum Type { Frequency, Resonance, Signal, Bypass, _Count }; }
		namespace SignedOutput { enum Type { Signal, _Count }; }
	}
	namespace Range
	{
		namespace SignedInput { enum Type { Value, Minimum, Maximum, _Count }; }
		namespace SignedOutput { enum Type { Value, _Count }; }
	}
	namespace Mixer
	{
		namespace SignedInput { enum Type { Signal0, Signal1, Signal2, Signal3, _Count }; }
		namespace SignedOutput { enum Type { Signal, _Count }; }
	}
	namespace PitchShift
	{
		namespace SignedInput { enum Type { Pitch, Shift, _Count }; }
		namespace SignedOutput { enum Type { Pitch, _Count }; }
	}
	namespace LFO
	{
		namespace SignedInput { enum Type { Pitch, Trigger, Duty, Level, Waveform, _Count }; }
		namespace SignedOutput { enum Type { Signal, _Count }; }
	}
	namespace Delay
	{
		namespace SignedInput { enum Type { Feedback, Signal, Period, _Count }; }
		namespace SignedOutput { enum Type { Signal, _Count }; }
	}
	namespace Arpeggiator
	{
		namespace SignedInput { enum Type { Period, Octaves, _Count }; }
		namespace SignedMultiInput { enum Type { Gate, Pitch, _Count }; }
		namespace SignedMultiOutput { enum Type { Gate, Pitch, _Count }; }
	}
}
enum class CommandType { StartGraph, InitGraph, AddMonoModule, AddPolyModule, AddConnection, EndGraph, SetUnsignedValue, SetSignedValue, SetMIDIData, StopMIDIPlayback };
enum class Error { None, UnknownCommandType, InvalidParameter, TooManyParameters, PinTypeMismatch, GraphAlreadyStarted };

enum class ConnectionType { None, Single, Multi, _Count };
enum class InstanceType { None, Mono, Poly, _Count };

}