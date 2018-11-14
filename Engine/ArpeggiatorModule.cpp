#include "ArpeggiatorModule.h"
#include "Util.h"

using namespace Engine;

ArpeggiatorModule::ArpeggiatorModule(int polyphony) : _polyphony(polyphony)
{
	_inputs.SetSize(Pin::Arpeggiator::Input::_Count);
	_multiInputs.SetSize(Pin::Arpeggiator::MultiInput::_Count);
	_multiOutputs.SetSize(Pin::Arpeggiator::MultiOutput::_Count);

	_pitches.SetSize(polyphony);
}

void ArpeggiatorModule::Update()
{
	MultiInput& gateInputs = _multiInputs[Pin::Arpeggiator::MultiInput::Gate];
	MultiInput& pitchInputs = _multiInputs[Pin::Arpeggiator::MultiInput::Pitch];
	Input& periodInput = _inputs[Pin::Arpeggiator::Input::Period];
	Input& octavesInput = _inputs[Pin::Arpeggiator::Input::Octaves];

	if (periodInput.HasChanged())
	{
		periodInput.ResetChanged();
		_period = static_cast<uint16_t>(periodInput.GetValue()) * Config::sampleRateMS;
	}

	if (octavesInput.HasChanged())
	{
		octavesInput.ResetChanged();
		_octaves = static_cast<uint16_t>(octavesInput.GetValue());
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
					float temp = _pitches[j + 1];
					_pitches[j + 1] = _pitches[j];
					_pitches[j] = temp;
				}


		if (_currentPitch >= _noteCount)
			_currentPitch = -1;

		if (_currentOutput >= 0)  // Stop current output. 
			_multiOutputs[Pin::Arpeggiator::MultiOutput::Gate][_currentOutput].SetValue(0);

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
			float pitch = _pitches[_currentPitch] + _currentOctave * Config::pitchPerOctave;
			_multiOutputs[Pin::Arpeggiator::MultiOutput::Gate][_currentOutput].SetValue(1);
			_multiOutputs[Pin::Arpeggiator::MultiOutput::Pitch][_currentOutput].SetValue(pitch);
			_waiting = false; // Don't sync until _noteCount == 0 again.
		}
		else
			_waiting = true;
	}
}
