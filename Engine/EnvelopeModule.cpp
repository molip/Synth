#include "EnvelopeModule.h"
#include "Util.h"

#include <Limits.h>

using namespace Engine;

EnvelopeModule::EnvelopeModule()
{
	_signedInputs.SetSize(Pin::Envelope::SignedInput::_Count);
	_signedOutputs.SetSize(Pin::Envelope::SignedOutput::_Count);
}

void EnvelopeModule::Update()
{
	SignedInput& gateInput = _signedInputs[Pin::Envelope::SignedInput::Gate];

	if (gateInput.HasChanged())
	{
		gateInput.ResetChanged();

		if (gateInput.GetValue() != 0) // Gate on.
		{
			const SignedInput& attackInput = _signedInputs[Pin::Envelope::SignedInput::Attack];
			const SignedInput& decayInput = _signedInputs[Pin::Envelope::SignedInput::Decay];
			const SignedInput& sustainInput = _signedInputs[Pin::Envelope::SignedInput::Sustain];

			uint16_t attack = static_cast<uint16_t>(attackInput.GetValue());
			uint16_t decay = static_cast<uint16_t>(decayInput.GetValue());
			_sustainLevel = FloatToFixed32(sustainInput.GetValue());

			_attackDelta = attack ? ULONG_MAX / (attack * Config::sampleRateMS) : 0;
			_decayDelta = decay ? (ULONG_MAX - _sustainLevel) / (decay * Config::sampleRateMS) : 0;

			_level = 0;
			_stage = Stage::Attack;
		}
		else // Gate off.
		{
			const SignedInput& releaseInput = _signedInputs[Pin::Envelope::SignedInput::Release];
		
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
			_stage = Stage::Decay;
		}
		break;
	case Stage::Decay:
		if (_decayDelta && _level > _sustainLevel + _decayDelta)
			_level -= _decayDelta;
		else
		{
			_level = _sustainLevel;
			_stage = Stage::Sustain;
		}
	case Stage::Sustain:
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

	SignedOutput& levelOutput = _signedOutputs[Pin::Envelope::SignedOutput::Level];
	levelOutput.SetValue(_level * Config::uint32ToFloat);
}
