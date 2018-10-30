#pragma once

#include <cmath>
#include <cstdint>

#ifdef _WIN32
	using byte = uint8_t;
	#define PROGMEM
	#define pgm_read_byte_near *
	#define SERIAL_PRINT(val)
	#define SERIAL_PRINTLN(val)
	#define OUTPUT_AUDIO(val)
	#define delay(val)
#else
	#define SERIAL_PRINT(val) Serial.println(val)
	#define SERIAL_PRINTLN(val) Serial.println(val)
	#define OUTPUT_AUDIO(val) analogWrite(A21, val)
#endif
