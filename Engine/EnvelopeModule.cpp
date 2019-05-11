#include "EnvelopeModule.h"
#include "Util.h"

#include <Limits.h>

using namespace Engine;

EnvelopeModule::EnvelopeModule()
{
	_inputs.SetSize(Pin::Envelope::Input::_Count);
	_outputs.SetSize(Pin::Envelope::Output::_Count);
}

void EnvelopeModule::Update()
{
	Input& gateInput = _inputs[Pin::Envelope::Input::Gate];

	auto startRelease = [&]()
	{
		const Input& releaseInput = _inputs[Pin::Envelope::Input::Release];

		if (uint32_t release = static_cast<uint32_t>(releaseInput.GetValue()))
		{
			_stage = Stage::Release;
			_releaseDelta = _level / (release * Config::sampleRateMS);
		}
		else
		{
			_stage = Stage::Off;
			_level = 0;
		}
	};

	if (gateInput.HasChanged())
	{
		gateInput.ResetChanged();

		if (_gate = gateInput.GetValue() != 0) // Gate on.
		{
			const Input& attackInput = _inputs[Pin::Envelope::Input::Attack];
			const Input& holdInput = _inputs[Pin::Envelope::Input::Hold];
			const Input& decayInput = _inputs[Pin::Envelope::Input::Decay];
			const Input& sustainInput = _inputs[Pin::Envelope::Input::Sustain];
			const Input& latchInput = _inputs[Pin::Envelope::Input::Latch];

			uint16_t attack = static_cast<uint16_t>(attackInput.GetValue());
			uint16_t hold = static_cast<uint16_t>(holdInput.GetValue());
			uint16_t decay = static_cast<uint16_t>(decayInput.GetValue());
			_sustainLevel = FloatToFixed32(sustainInput.GetValue());
			_latch = latchInput.GetValue() > 0;

			_attackDelta = attack ? ULONG_MAX / (attack * Config::sampleRateMS) : 0;
			_decayDelta = decay ? (ULONG_MAX - _sustainLevel) / (decay * Config::sampleRateMS) : 0;
			_holdRemaining = hold * Config::sampleRateMS;

			_level = 0;
			_stage = Stage::Attack;
		}
		else if (!_latch) // Gate off.
		{
			startRelease();
		}
	}


	switch (_stage)
	{
	case Stage::Attack:
		if (_attackDelta && _level < ULONG_MAX - _attackDelta)
			_level += _attackDelta;
		else
		{
			_level = ULONG_MAX;
			_stage = Stage::Hold;
		}
		break;
	case Stage::Hold:
		if (_holdRemaining == 0)
			_stage = Stage::Decay;
		else
			--_holdRemaining;
		break;
	case Stage::Decay:
		if (_decayDelta && _level > _sustainLevel + _decayDelta)
			_level -= _decayDelta;
		else
		{
			_level = _sustainLevel;
			_stage = Stage::Sustain;
		}
		break;
	case Stage::Sustain:
		if (_latch && !_gate)
			startRelease();

		_latch = false; // Release when gate goes off.
		break;
	case Stage::Release:
		if (_level > _releaseDelta)
			_level -= _releaseDelta;
		else
		{
			_level = 0;
			_stage = Stage::Off;
		}
		break;
	default:
		break;
	}

	Output& levelOutput = _outputs[Pin::Envelope::Output::Level];
	levelOutput.SetValue(_level * Config::uint32ToFloat);
}
