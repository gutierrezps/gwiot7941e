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



bool Gwiot7941e::update()
{
    char buff[GWIOT_7941E_PACKET_SIZE];

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
    if (buff[9] != GWIOT_7941E_PACKET_END) return false;

    // calculate checksum (excluding start and end bytes)
    uint8_t checksum = 0;
    for (uint8_t i = 1; i <= 8; ++i) {
        checksum ^= buff[i];
    }
    if (checksum) return false;

    // extract tag id from message
    lastTagId_ = 0;
    for (uint8_t i = 0; i <= 3; ++i) {
        uint32_t val = (uint8_t) buff[i+4];
        lastTagId_ |= val << (8 * (3 - i));
    }

    return lastTagId_;
}
