#include "ArpeggiatorModule.h"
#include "Util.h"

using namespace Engine;

ArpeggiatorModule::ArpeggiatorModule(int polyphony, Array<MultiOutput>& outputs) :
	_polyphony(polyphony), _outputs(outputs)
{
	_pitches.Reserve(polyphony);
	_midiInputs.SetSize(Pin::MIDI::MultiOutput::_Count);

	_octaves = 1;
}

void ArpeggiatorModule::Connect(Array<MultiOutput>& midiOutputs)
{
	for (int i = 0; i < _midiInputs.GetSize(); ++i)
	{
		_midiInputs[i].SetSize(_polyphony);
		for (int j = 0; j < _polyphony; ++j)
			_midiInputs[i][j].Connect(midiOutputs[i][j]);
	}
}

void ArpeggiatorModule::SetPeriod(uint16_t period)
{ 
	_period = period * Config::sampleRateMS;
}

void ArpeggiatorModule::SetDuty(float duty)
{
	_duty = duty;
}

void ArpeggiatorModule::SetOctaves(uint16_t octaves)
{ 
	_octaves = octaves;
	if (_currentOctave >= octaves)
		_currentOctave = 0;
}

void ArpeggiatorModule::Update()
{
	MultiInput& gateInputs = _midiInputs[Pin::MIDI::MultiOutput::Gate];
	MultiInput& pitchInputs = _midiInputs[Pin::MIDI::MultiOutput::Pitch];

	bool inputsChanged = false;
	for (int i = 0; i < _polyphony; ++i)
		if ((inputsChanged = gateInputs[i].HasChanged()))
			break;

	bool sync = false;

	if (inputsChanged)
	{
		_pitches.Clear();

		for (int i = 0; i < _polyphony; ++i)
		{
			gateInputs[i].ResetChanged();
			if (gateInputs[i].GetValue())
				_pitches.Push(pitchInputs[i].GetValue());
		}

		if (!_noteCount && !_pitches.IsEmpty())
		{
			sync = _waiting;
			_currentOctave = 0;
			_currentPitch = -1;
		}
		
		_noteCount = _pitches.GetSize();
		
		_pitches.Sort();
	}

	++_phase;

	if (_phase >= _periodOn && _currentOutput >= 0)
		_outputs[Pin::MIDI::MultiOutput::Gate][_currentOutput].SetValue(0);

	if (sync || _phase >= _period)
	{
		_phase = 0;
		_periodOn = static_cast<uint32_t>(_duty * 0.01f * _period);

		if (_currentPitch >= _noteCount)
			_currentPitch = -1;

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
			float pitch = _pitches[_currentPitch] + _currentOctave * 12;
			_outputs[Pin::MIDI::MultiOutput::Gate][_currentOutput].SetValue(1);
			_outputs[Pin::MIDI::MultiOutput::Pitch][_currentOutput].SetValue(pitch);
			_waiting = false; // Don't sync until _noteCount == 0 again.
		}
		else
			_waiting = true;
	}
}
