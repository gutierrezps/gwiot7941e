/*
 * A simple library to interface with Gwiot 7941E RFID reader.
 * 
 * Arad Eizen (https://github.com/arduino12)
 * Gutierrez PS (https://github.com/gutierrezps)
 */

#include "Gwiot7941e.h"
#include <Arduino.h>


void Gwiot7941e::begin(Stream *stream)
{
	_stream = stream;
	if (!_stream)
		return;
	_stream->setTimeout(GWIOT_7941E_READ_TIMEOUT);
}

void Gwiot7941e::begin(int rx_pin, uint8_t uart_nr)
{
	/* init serial port to 7941E baud, without TX, and 20ms read timeout */
	end();
#if defined(ARDUINO_ARCH_ESP32)
	_stream = _hardware_serial = new HardwareSerial(uart_nr);
	_hardware_serial->begin(GWIOT_7941E_BAUDRATE, SERIAL_8N1, rx_pin, -1);
#elif defined(ARDUINO_ARCH_ESP8266)
	if (rx_pin == 13) {
		_stream = _hardware_serial = &Serial;
		_hardware_serial->begin(GWIOT_7941E_BAUDRATE, SERIAL_8N1, SERIAL_RX_ONLY);
		if (uart_nr)
			_hardware_serial->swap();
	}
#endif
#ifdef GWIOT_7941E_SOFTWARE_SERIAL
	if (!_stream) {
		_stream = _software_serial = new SoftwareSerial(rx_pin, -1);
		_software_serial->begin(GWIOT_7941E_BAUDRATE);
	}
#endif
	begin(_stream);
}

void Gwiot7941e::end()
{
	_stream = NULL;
#ifdef GWIOT_7941E_HARDWARE_SERIAL
	if (_hardware_serial)
		_hardware_serial->end();
#endif
#ifdef GWIOT_7941E_SOFTWARE_SERIAL
	if (_software_serial)
		_software_serial->end();
#endif
}

bool Gwiot7941e::update(void)
{
	char buff[GWIOT_7941E_PACKET_SIZE];
	uint32_t tag_id;
	uint8_t checksum;

	if (!_stream)
		return false;

	if (!_stream->available())
		return false;

	/* if a packet doesn't begin with the right byte, remove that byte */
	if (_stream->peek() != GWIOT_7941E_PACKET_BEGIN && _stream->read())
		return false;

	/* if read a packet with the wrong size, drop it */
	if (GWIOT_7941E_PACKET_SIZE != _stream->readBytes(buff, GWIOT_7941E_PACKET_SIZE))
		return false;

	/* if a packet doesn't end with the right byte, drop it */
	if (buff[13] != GWIOT_7941E_PACKET_END)
		return false;

	/* add null and parse checksum */
	buff[13] = 0;
	checksum = strtol(buff + 11, NULL, 16);
	/* add null and parse tag_id */
	buff[11] = 0;
	tag_id = strtol(buff + 3, NULL, 16);
	/* add null and parse version (needs to be xored with checksum) */
	buff[3] = 0;
	checksum ^= strtol(buff + 1, NULL, 16);

	/* xore the tag_id and validate checksum */
	for (uint8_t i = 0; i < 32; i += 8)
		checksum ^= ((tag_id >> i) & 0xFF);
	if (checksum)
		return false;

	/* if a new tag appears- return it */
	if (_last_tag_id != tag_id) {
		_last_tag_id = tag_id;
		_last_read_ms = 0;
	}
	/* if the old tag is still here set tag_id to zero */
	if (is_tag_near())
		tag_id = 0;
	_last_read_ms = millis();

	_tag_id = tag_id;
	return tag_id;
}

bool Gwiot7941e::is_tag_near(void)
{
	return millis() - _last_read_ms < GWIOT_7941E_NEXT_READ_MS;
}

uint32_t Gwiot7941e::get_tag_id(void)
{
	uint32_t tag_id = _tag_id;
	_tag_id = 0;
	return tag_id;
}
