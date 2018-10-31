#pragma once

#include <cmath>
#include <cstdint>

using byte = uint8_t;

#ifdef _WIN32
	#define SERIAL_PRINT(val)
	#define SERIAL_PRINTLN(val)
	#define OUTPUT_AUDIO(val)
	#define delay(val)
#else
	#include "kinetis.h"
	#include "core_pins.h"
	#include "usb_serial.h"

	#define SERIAL_PRINT(val) Serial.println(val)
	#define SERIAL_PRINTLN(val) Serial.println(val)
	#define OUTPUT_AUDIO(val) analogWrite(A21, val)
#endif
