#include "OutputModule.h"

void OutputModule::Update()
{
	analogWrite(A21, (_input.GetValue() + 0x8000) >> 4);
}
