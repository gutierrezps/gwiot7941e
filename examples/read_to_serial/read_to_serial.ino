/**
 * A simple example to interface with Gwiot 7941E RFID reader.
 *
 * Connect the 7941E to VCC=5V, GND=GND, TX=any GPIO (this case GPIO-04)
 * Note that the 7941E's TX line is 3.3V level,
 * so it's save to use with both AVR* and ESP* microcontrollers.
 *
 * Arad Eizen (https://github.com/arduino12)
 * Gutierrez PS (https://github.com/gutierrezps)
 */

#include <Gwiot7941e.h>

#define GWIOT_7941E_RX_PIN 4
#define READ_LED_PIN 13

Gwiot7941e gwiot7941e;

void setup()
{
    Serial.begin(115200);

    pinMode(READ_LED_PIN, OUTPUT);
    digitalWrite(READ_LED_PIN, LOW);

    gwiot7941e.begin(GWIOT_7941E_RX_PIN);

    Serial.println("\nPlace RFID tag near the 7941E...");
}

void loop()
{
    // if non-zero tag_id, update() returns true- a new tag is near!
    if (gwiot7941e.update()) {
        Serial.println(gwiot7941e.getTagId(), HEX);
    }

    delay(10);
}
