/**
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



#define GWIOT_7941E_BAUDRATE        9600
#define GWIOT_7941E_PACKET_SIZE     10
#define GWIOT_7941E_PACKET_BEGIN    0x02
#define GWIOT_7941E_PACKET_END      0x03
#define GWIOT_7941E_READ_TIMEOUT    20



class Gwiot7941e {
public:
    void begin(Stream *stream);
    void begin(int rxPin, uint8_t uartNum=1);
    void end();
    bool update();
    uint32_t getLastTagId() { return lastTagId_; }

private:
#ifdef GWIOT_7941E_HARDWARE_SERIAL
    HardwareSerial *hardwareSerial_ = NULL;
#endif

#ifdef GWIOT_7941E_SOFTWARE_SERIAL
    SoftwareSerial *softwareSerial_ = NULL;
#endif

    Stream *stream_ = NULL;
    uint32_t lastTagId_ = 0;
};

#endif
