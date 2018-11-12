#pragma once

#include "Types.h"

#include <map>

namespace Synth
{
	class EngineTest
	{
	public:
		static EngineTest& Instance();
		bool Test(const Buffer& buffer);

	private:
		EngineTest();
	};
}
