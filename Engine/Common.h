#pragma once

namespace Engine
{
enum class ModuleType { None, MIDI, Envelope, Oscillator, PolyMixer, Mixer, Target, Filter, Range, PitchShift, LFO, Delay, Arpeggiator, _Count };

namespace Pin
{
	namespace MIDI
	{
		namespace MultiOutput { enum Type { Pitch, Gate, _Count }; }
	}
	namespace Envelope 
	{
		namespace Input { enum Type { Gate, Sustain, Attack, Decay, Release, _Count }; }
		namespace Output { enum Type { Level, _Count }; }
	}
	namespace Oscillator 
	{
		namespace Input { enum Type { Pitch, PhaseMod, Level, Waveform, _Count }; }
		namespace Output { enum Type { Signal, _Count }; }
	}
	namespace PolyMixer 
	{
		namespace MultiInput { enum Type { Signal, _Count }; }
		namespace Output { enum Type { Signal, _Count }; }
	}
	namespace Target
	{
		namespace Input { enum Type { Signal, _Count }; }
	}
	namespace Filter
	{
		namespace Input { enum Type { Frequency, Resonance, Signal, Bypass, _Count }; }
		namespace Output { enum Type { Signal, _Count }; }
	}
	namespace Range
	{
		namespace Input { enum Type { Value, Minimum, Maximum, _Count }; }
		namespace Output { enum Type { Value, _Count }; }
	}
	namespace Mixer
	{
		namespace Input { enum Type { Signal0, Signal1, Signal2, Signal3, _Count }; }
		namespace Output { enum Type { Signal, _Count }; }
	}
	namespace PitchShift
	{
		namespace Input { enum Type { Pitch, Shift, _Count }; }
		namespace Output { enum Type { Pitch, _Count }; }
	}
	namespace LFO
	{
		namespace Input { enum Type { Pitch, Trigger, Duty, Level, Waveform, _Count }; }
		namespace Output { enum Type { Signal, _Count }; }
	}
	namespace Delay
	{
		namespace Input { enum Type { Feedback, Signal, Period, _Count }; }
		namespace Output { enum Type { Signal, _Count }; }
	}
	namespace Arpeggiator
	{
		namespace Input { enum Type { Period, Octaves, _Count }; }
		namespace MultiInput { enum Type { Gate, Pitch, _Count }; }
		namespace MultiOutput { enum Type { Gate, Pitch, _Count }; }
	}
}
enum class CommandType { StartGraph, InitGraph, AddMonoModule, AddPolyModule, AddConnection, EndGraph, SetUnsignedValue, SetSignedValue, SetMIDIData, SetAllNotesOn, StopMIDIPlayback };
enum class Error { None, UnknownCommandType, InvalidParameter, TooManyParameters, PinTypeMismatch, GraphAlreadyStarted };

enum class ConnectionType { None, Single, Multi, _Count };
enum class InstanceType { None, Mono, Poly, _Count };

}