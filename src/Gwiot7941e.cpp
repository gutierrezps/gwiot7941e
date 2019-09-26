/**
 * A simple library to interface with Gwiot 7941E RFID reader.
 * 
 * Arad Eizen (https://github.com/arduino12)
 * Gutierrez PS (https://github.com/gutierrezps)
 */

#include <Arduino.h>
#include "Gwiot7941e.h"



void Gwiot7941e::begin(Stream *stream)
{
    stream_ = stream;
    if (!stream_) return;
    stream_->setTimeout(GWIOT_7941E_READ_TIMEOUT);
}



void Gwiot7941e::begin(int rxPin, uint8_t uartNum)
{
    // init serial port to 7941E baud, without TX, and 20ms read timeout
    end();

#if defined(ARDUINO_ARCH_ESP32)
    stream_ = hardwareSerial_ = new HardwareSerial(uartNum);
    hardwareSerial_->begin(GWIOT_7941E_BAUDRATE, SERIAL_8N1, rxPin, -1);

#elif defined(ARDUINO_ARCH_ESP8266)
    if (rxPin == 13) {
        stream_ = hardwareSerial_ = &Serial;
        hardwareSerial_->begin(GWIOT_7941E_BAUDRATE, SERIAL_8N1, SERIAL_RX_ONLY);
        if (uartNum) hardwareSerial_->swap();
    }
#endif

#ifdef GWIOT_7941E_SOFTWARE_SERIAL
    if (!stream_) {
        stream_ = softwareSerial_ = new SoftwareSerial(rxPin, -1);
        softwareSerial_->begin(GWIOT_7941E_BAUDRATE);
    }
#endif

    begin(stream_);
}



void Gwiot7941e::end()
{
    stream_ = NULL;

#ifdef GWIOT_7941E_HARDWARE_SERIAL
    if (hardwareSerial_) hardwareSerial_->end();
#endif

#ifdef GWIOT_7941E_SOFTWARE_SERIAL
    if (softwareSerial_) softwareSerial_->end();
#endif
}



bool Gwiot7941e::update(void)
{
    char buff[GWIOT_7941E_PACKET_SIZE];
    uint32_t tagId;
    uint8_t checksum;

    if (!stream_) return false;

    if (!stream_->available()) return false;

    // if a packet doesn't begin with the right byte, remove that byte
    if (stream_->peek() != GWIOT_7941E_PACKET_BEGIN && stream_->read()) {
        return false;
    }

    // if read a packet with the wrong size, drop it
    if (GWIOT_7941E_PACKET_SIZE != stream_->readBytes(buff, GWIOT_7941E_PACKET_SIZE)) {
        return false;
    }

    // if a packet doesn't end with the right byte, drop it
    if (buff[13] != GWIOT_7941E_PACKET_END) {
        return false;
    }

    // add null and parse checksum
    buff[13] = 0;
    checksum = strtol(buff + 11, NULL, 16);
    // add null and parse tagId
    buff[11] = 0;
    tagId = strtol(buff + 3, NULL, 16);
    // add null and parse version (needs to be xored with checksum)
    buff[3] = 0;
    checksum ^= strtol(buff + 1, NULL, 16);

    // XOR the tagId and validate checksum
    for (uint8_t i = 0; i < 32; i += 8) {
        checksum ^= ((tagId >> i) & 0xFF);
    }
    if (checksum) return false;

    // if a new tag appears- return it
    if (lastTagId_ != tagId) {
        lastTagId_ = tagId;
        lastReadMillis_ = 0;
    }
    // if the old tag is still here set tagId to zero
    if (isTagNear()) tagId = 0;
    lastReadMillis_ = millis();

    tagId_ = tagId;
    return tagId;
}



bool Gwiot7941e::isTagNear(void)
{
    return millis() - lastReadMillis_ < GWIOT_7941E_NEXT_READ_MS;
}



uint32_t Gwiot7941e::getTagId(void)
{
    uint32_t tagId = tagId_;
    tagId_ = 0;
    return tagId;
}
