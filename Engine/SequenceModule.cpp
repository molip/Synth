#include "SequenceModule.h"
#include "Util.h"

using namespace Engine;

SequenceModule::SequenceModule()
{
	_inputs.SetSize(Pin::Sequence::Input::_Count);
	_outputs.SetSize(Pin::Sequence::Output::_Count);
	_fields.SetSize(Pin::Sequence::Field::_Count);
}

void SequenceModule::Update()
{
	if (++_divide < 256) // [1/32 Hz, 48 Hz]
		return;

	_divide = 0;

	Input& pitchInput = _inputs[Pin::Sequence::Input::Pitch];
	Output& valueOutput = _outputs[Pin::Sequence::Output::Value];
	Output& gateOutput = _outputs[Pin::Sequence::Output::Gate];
	Field& dataField = _fields[Pin::Sequence::Field::Sequence];

	const Field::DataType* data = dataField.GetData();

	if (dataField.HasChanged())
	{
		dataField.ResetChanged();
		_index = 0;

		if (!data)
			valueOutput.SetValue(0);
	}

	if (!data)
		return;

	if (pitchInput.HasChanged())
	{
		pitchInput.ResetChanged();
		_phaseDelta = ::PitchToPhaseDelta(pitchInput.GetValue());
	}

	uint16_t newPhase = _phase + _phaseDelta;
	
	if (newPhase < _phase)
	{
		valueOutput.SetValue((*data)[_index]);
		gateOutput.SetValue(1, true);

		_index = (_index + 1) % data->GetSize();

	}

	_phase = newPhase;
}
