/**
 * A simple example to interface with Gwiot 7941E RFID reader using AltSoftSerial.
 * We use AltSoftSerial uart instead of the default software uart driver.
 *
 * Connections:
 *     | Board            | RX Pin | PWM Unusable | Pin Unusable |
 *     |------------------+--------+--------------+--------------|
 *     | Teensy 3.0 & 3.1 |     20 |           22 |           21 |
 *     | Teensy 2.0       |     10 |            - |            9 |
 *     | Teensy++ 2.0     |      4 |       26, 27 |           25 |
 *     | Arduino Uno      |      8 |           10 |            9 |
 *     | Arduino Leonardo |     13 |            - |            5 |
 *     | Arduino Mega     |     48 |       44, 45 |           46 |
 *     | Wiring-S         |      6 |            4 |            5 |
 *     | Sanguino         |     14 |           12 |           13 |
 *
 * Arad Eizen (https://github.com/arduino12)
 * Gutierrez PS (https://github.com/gutierrezps)
 */

#include <Gwiot7941e.h>
#include <AltSoftSerial.h>

#define GWIOT_7941E_RX_PIN 8
#define READ_LED_PIN 13

Gwiot7941e gwiot7941e;
AltSoftSerial altSoftSerial;


void setup()
{
    Serial.begin(115200);

    pinMode(READ_LED_PIN, OUTPUT);
    digitalWrite(READ_LED_PIN, LOW);

    altSoftSerial.begin(GWIOT_7941E_BAUDRATE);
    gwiot7941e.begin(&altSoftSerial);

    Serial.println("\nPlace RFID tag near the 7941E...");
}



void loop()
{
    // if non-zero tag_id, update() returns true - a tag was read!
    if (gwiot7941e.update()) {
        Serial.println(gwiot7941e.getLastTagId(), HEX);
    }

    delay(10);
}
