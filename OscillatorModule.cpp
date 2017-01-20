#include "OscillatorModule.h"

uint16_t OscillatorModule::PitchToPhaseDelta(unsigned_t pitch) 
{
	int8_t midiNote = pitch >> 9; // 7 bit;
	int octave = midiNote / 12;
	int pitchIndex = midiNote % 12;
	
	float freq = Config::freqs[pitchIndex];
	for (int i = 0; i < octave; ++i)
		freq = freq + freq;

	Serial.print("freq: "); Serial.println(freq);

	return uint16_t(freq * Config::freqToDeltaScale);
}

void OscillatorModule::Update()
{
	unsigned_t level = _levelInput._source->GetValue();
	if (level == 0)
	{
		_output.SetValue(0);
		_phase = 0;
		return;
	}

	if (_pitchInput._source->HasChanged())
	{
		_phaseDelta = PitchToPhaseDelta(_pitchInput._source->GetValue());
	}

	_phase += _phaseDelta;

	uint16_t output = ((_phase & 0x8000) ? 0xffff - _phase : _phase) << 1; // Triangle.

	//Serial.println(output);

	_output.SetValue(((output - 0x8000) * level) >> 16); // Signed.
	//_output.SetValue((output - 0x8000); // Signed.
}
