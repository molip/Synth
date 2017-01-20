#include "OutputModule.h"

void OutputModule::Update()
{
	analogWrite(A21, (_input._source->GetValue() + 0x8000) >> 4);
}
