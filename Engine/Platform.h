#pragma once

#include <cmath>
#include <cstdint>

#ifndef _WIN32
#include "kinetis.h"
#include "core_pins.h"
#include "usb_serial.h"
#endif

namespace Engine
{
using byte = uint8_t;

#ifdef _WIN32
	#define SERIAL_PRINT(val)
	#define SERIAL_PRINTLN(val)
	#define OUTPUT_AUDIO(val)
	#define delay(val)
#else
	#define SERIAL_PRINT(val) Serial.print(val)
	#define SERIAL_PRINTLN(val) Serial.println(val)
	#define OUTPUT_AUDIO(val) analogWrite(A21, val)
#endif
}