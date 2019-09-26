/*
 * A simple example to interface with Gwiot 7941E RFID reader using esp8266.
 * We use hardware uart "Serial" instead of the default software uart driver.
 *
 * Note:
 *     The esp8266 let us use 1.5 uarts:
 *     Serial=uart0=rx+tx, Serial1=uart1=tx only (rx pin is used as flash io).
 *     Here we sawp the uart pins so uart0_rx goes to the 7941E_tx,
 *     and uart1_tx goes to what used to be uart0_tx-
 *     so debug message goes from Serial1.print(...) to the pc debug terminal.
 *     https://github.com/esp8266/Arduino/blob/master/doc/reference.rst#serial
 *
 * Connections:
 *     | esp8266 | nodemcu | 7941E | notes                               |
 *     |---------+---------+-------|-------------------------------------|
 *     | GPIO-01 | TX      |       | This is TXD0, connect it to GPIO-02 |
 *     | GPIO-02 | D4      |       | This is TXD1, connect it to GPIO-01 |
 *     | GPIO-03 | RX      |       | Leave it unconnected for flashing   |
 *     | GPIO-13 | D7      | TX    | TX level is same 3.3V as esp8266    |
 *     |         | VU (5V) | VCC   | The 7941E must be powered with 5V   |
 *     | GND     | GND     | GND   |                                     |
 *
 *     * GPIO-01 to GPIO-02 is for debug terminal output.
 *
 * Arad Eizen (https://github.com/arduino12)
 * Gutierrez PS (https://github.com/gutierrezps)
 */

#include <Gwiot7941e.h>

#define GWIOT_7941E_RX_PIN 13 // can be only 13 - on esp8266 force hardware uart!
#define READ_LED_PIN 16

Gwiot7941e gwiot7941e;

void setup()
{
	/* Serial1 is the debug! remember to bridge GPIO-01 to GPIO-02 */
	Serial1.begin(115200);

	pinMode(READ_LED_PIN, OUTPUT);
	digitalWrite(READ_LED_PIN, LOW);

	gwiot7941e.begin(GWIOT_7941E_RX_PIN);

	Serial1.println("\nPlace RFID tag near the 7941E...");
}

void loop()
{
	/* if non-zero tag_id, update() returns true- a new tag is near! */
	if (gwiot7941e.update())
		Serial1.println(gwiot7941e.get_tag_id(), HEX);

	digitalWrite(READ_LED_PIN, gwiot7941e.is_tag_near());

	delay(10);
}
