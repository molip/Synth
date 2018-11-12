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
	void SerialPrint(const char* val);
	void SerialPrint(const int val);
	void SerialPrintLn(const char* val);
	void SerialPrintLn(const int val);
	void OutputAudio(const int val);

	#define SERIAL_PRINT(val) SerialPrint(val)
	#define SERIAL_PRINTLN(val) SerialPrintLn(val)
	#define OUTPUT_AUDIO(val) OutputAudio(val)
	#define delay(val)
#else
	#define SERIAL_PRINT(val) Serial.print(val)
	#define SERIAL_PRINTLN(val) Serial.println(val)
	#define OUTPUT_AUDIO(val) analogWrite(A21, val)
#endif
}