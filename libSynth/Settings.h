#pragma once

namespace Synth
{
	class Settings
	{
	public:
		unsigned int polyphony = 4;
		bool hold = false;
		bool arpEnabled = false;
		uint16_t arpPeriod = 100;
		uint16_t arpDuty = 90; // Percent.
		uint16_t arpOctaves = 1;
	};
}
