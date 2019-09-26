/*
 * A simple library to interface with Gwiot 7941E RFID reader.
 *
 * Arad Eizen (https://github.com/arduino12)
 * Gutierrez PS (https://github.com/gutierrezps)
 */

#ifndef _GWIOT_7941E_h_
#define _GWIOT_7941E_h_

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
	#define GWIOT_7941E_HARDWARE_SERIAL
#endif

#if !defined(ARDUINO_ARCH_ESP32)
	#define GWIOT_7941E_SOFTWARE_SERIAL
#endif

#ifdef GWIOT_7941E_HARDWARE_SERIAL
	#include <HardwareSerial.h>
#endif
#ifdef GWIOT_7941E_SOFTWARE_SERIAL
	#include <SoftwareSerial.h>
#endif

#include <Stream.h>

#define GWIOT_7941E_BAUDRATE		9600
#define GWIOT_7941E_PACKET_SIZE		14
#define GWIOT_7941E_PACKET_BEGIN	0x02
#define GWIOT_7941E_PACKET_END		0x03
#define GWIOT_7941E_NEXT_READ_MS	220
#define GWIOT_7941E_READ_TIMEOUT	20

class Gwiot7941e
{
	public:
		void begin(Stream *stream);
		void begin(int rx_pin, uint8_t uart_nr=1);
		void end(void);
		bool update(void);
		uint32_t get_tag_id(void);
		bool is_tag_near(void);
	private:
#ifdef GWIOT_7941E_HARDWARE_SERIAL
		HardwareSerial *_hardware_serial = NULL;
#endif
#ifdef GWIOT_7941E_SOFTWARE_SERIAL
		SoftwareSerial *_software_serial = NULL;
#endif
		Stream *_stream = NULL;
		uint32_t _tag_id = 0;
		uint32_t _last_tag_id = 0;
		uint32_t _last_read_ms = 0;
};

#endif
