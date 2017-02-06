/*
Based on http://www.kerrywong.com/2012/07/25/code-for-mcp4821-mcp4822/

MCP4822 pins: 
	1	+5V
	2	PIN_CS
	3	Digital 13
	4	Digital 11
	5	GND
	6	N/C
	7	10uF -> 1k -> out
	8	GND
*/

#include <SPI.h>

const int PIN_CS = 10;
const int GAIN_1 = 0x1;
const int GAIN_2 = 0x0;

void MCP4822_init()
{
	pinMode(PIN_CS, OUTPUT);
	SPI.begin();  
	SPI.setClockDivider(SPI_CLOCK_DIV2);
}

//assuming single channel, gain=2
void MCP4822_setOutput(unsigned int val)
{
	byte lowByte = val & 0xff;
	byte highByte = ((val >> 8) & 0xff) | 0x10;

	PORTB &= 0xfb;
	SPI.transfer(highByte);
	SPI.transfer(lowByte);
	PORTB |= 0x4;
}
 
void MCP4822_setOutput(byte channel, byte gain, byte shutdown, unsigned int val)
{
	byte lowByte = val & 0xff;
	byte highByte = ((val >> 8) & 0xff) | channel << 7 | gain << 5 | shutdown << 4;

	PORTB &= 0xfb;
	SPI.transfer(highByte);
	SPI.transfer(lowByte);
	PORTB |= 0x4;
}

