#include "EnvelopeModule.h"

#include <Limits.h>

void EnvelopeModule::Update()
{
	if (_gateInput._source->HasChanged())
	{
		if (_gateInput._source->GetValue() != 0) // Gate on.
		{
			unsigned_t attack = _attackInput._source->GetValue();
			unsigned_t decay = _decayInput._source->GetValue();
			_sustainLevel = _sustainInput._source->GetValue() << 16;

			_attackDelta = attack ? ULONG_MAX / (attack * Config::sampleRate * 0.001) : 0;
			_decayDelta = decay ? (ULONG_MAX - _sustainLevel) / (decay * Config::sampleRate * 0.001) : 0;

			_level = 0;
			_stage = Stage::Attack;
		}
		else // Gate off.
		{
			if (uint32_t release = _releaseInput._source->GetValue())
			{
				_stage = Stage::Release;
				_releaseDelta = _level / (release * Config::sampleRate * 0.001);
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

	_output.SetValue(_level >> 16);
}
