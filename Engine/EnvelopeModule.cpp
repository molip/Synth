#include "EnvelopeModule.h"

#include <Limits.h>

EnvelopeModule::EnvelopeModule()
{
	_unsignedInputs.SetSize(Pin::Envelope::UnsignedInput::_Count);
	_unsignedOutputs.SetSize(Pin::Envelope::UnsignedOutput::_Count);
}

void EnvelopeModule::Update()
{
	UnsignedInput& gateInput = _unsignedInputs[Pin::Envelope::UnsignedInput::Gate];

	if (gateInput.HasChanged())
	{
		gateInput.ResetChanged();

		if (gateInput.GetValue() != 0) // Gate on.
		{
			const UnsignedInput& attackInput = _unsignedInputs[Pin::Envelope::UnsignedInput::Attack];
			const UnsignedInput& decayInput = _unsignedInputs[Pin::Envelope::UnsignedInput::Decay];
			const UnsignedInput& sustainInput = _unsignedInputs[Pin::Envelope::UnsignedInput::Sustain];

			unsigned_t attack = attackInput.GetValue();
			unsigned_t decay = decayInput.GetValue();
			_sustainLevel = sustainInput.GetValue() << 16;

			_attackDelta = attack ? ULONG_MAX / (attack * Config::sampleRateMS) : 0;
			_decayDelta = decay ? (ULONG_MAX - _sustainLevel) / (decay * Config::sampleRateMS) : 0;

			_level = 0;
			_stage = Stage::Attack;
		}
		else // Gate off.
		{
			const UnsignedInput& releaseInput = _unsignedInputs[Pin::Envelope::UnsignedInput::Release];
		
			if (uint32_t release = releaseInput.GetValue())
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

	UnsignedOutput& levelOutput = _unsignedOutputs[Pin::Envelope::UnsignedOutput::Level];
	levelOutput.SetValue(_level >> 16);
}
