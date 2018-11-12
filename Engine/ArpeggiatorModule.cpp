#include "ArpeggiatorModule.h"

using namespace Engine;

ArpeggiatorModule::ArpeggiatorModule(int polyphony) : _polyphony(polyphony)
{
	_unsignedInputs.SetSize(Pin::Arpeggiator::UnsignedInput::_Count);
	_unsignedMultiInputs.SetSize(Pin::Arpeggiator::UnsignedMultiInput::_Count);
	_unsignedMultiOutputs.SetSize(Pin::Arpeggiator::UnsignedMultiOutput::_Count);

	_pitches.SetSize(polyphony);
}

void ArpeggiatorModule::Update()
{
	UnsignedMultiInput& gateInputs = _unsignedMultiInputs[Pin::Arpeggiator::UnsignedMultiInput::Gate];
	UnsignedMultiInput& pitchInputs = _unsignedMultiInputs[Pin::Arpeggiator::UnsignedMultiInput::Pitch];
	UnsignedInput& periodInput = _unsignedInputs[Pin::Arpeggiator::UnsignedInput::Period];
	UnsignedInput& octavesInput = _unsignedInputs[Pin::Arpeggiator::UnsignedInput::Octaves];

	if (periodInput.HasChanged())
	{
		periodInput.ResetChanged();
		_period = periodInput.GetValue() * Config::sampleRateMS;
	}

	if (octavesInput.HasChanged())
	{
		octavesInput.ResetChanged();
		_octaves = octavesInput.GetValue();
		if (_currentOctave >= _octaves)
			_currentOctave = 0;
	}

	bool inputsChanged = false;
	for (int i = 0; i < _polyphony; ++i)
		if ((inputsChanged = gateInputs[i].HasChanged()))
			break;

	bool sync = false;
	if (inputsChanged)
	{
		int newNoteCount = 0;
		for (int i = 0; i < _polyphony; ++i)
		{
			gateInputs[i].ResetChanged();
			if (gateInputs[i].GetValue())
				_pitches[newNoteCount++] = pitchInputs[i].GetValue();
		}

		if (!_noteCount && newNoteCount)
		{
			sync = _waiting;
			_currentOctave = 0;
			_currentPitch = -1;
		}
		_noteCount = newNoteCount;
	}

	if (sync || ++_phase >= _period)
	{
		_phase = 0;

		for (int i = 0; i < _noteCount - 1; ++i)
			for (int j = 0; j < _noteCount - i - 1; ++j)
				if (_pitches[j] > _pitches[j + 1])
				{
					unsigned_t temp = _pitches[j + 1];
					_pitches[j + 1] = _pitches[j];
					_pitches[j] = temp;
				}


		if (_currentPitch >= _noteCount)
			_currentPitch = -1;

		if (_currentOutput >= 0)  // Stop current output. 
			_unsignedMultiOutputs[Pin::Arpeggiator::UnsignedMultiOutput::Gate][_currentOutput].SetValue(0);

		_currentOutput = (_currentOutput + 1) % _polyphony;
		
		++_currentPitch;
		if (_currentPitch >= _noteCount)
		{
			_currentPitch = 0;
			if (_noteCount)
				_currentOctave = (_currentOctave + 1) % _octaves;
		}

		if (_noteCount)
		{
			unsigned_t pitch = _pitches[_currentPitch] + static_cast<unsigned_t>(_currentOctave * Config::pitchPerOctave);
			_unsignedMultiOutputs[Pin::Arpeggiator::UnsignedMultiOutput::Gate][_currentOutput].SetValue(0xffff);
			_unsignedMultiOutputs[Pin::Arpeggiator::UnsignedMultiOutput::Pitch][_currentOutput].SetValue(pitch);
			_waiting = false; // Don't sync until _noteCount == 0 again.
		}
		else
			_waiting = true;
	}
}
