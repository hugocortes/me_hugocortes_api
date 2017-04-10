/**
 * @author Hugo Cortes
 * @file RF24Payload.h
 *
 * nRF24 payloads maximum size 32 bytes. 
 */

#ifndef RF24PAYLOAD_H
#define RF24PAYLOAD_H

#include <stdint.h>

struct RF24Payload {
    unsigned cmd:4;
    unsigned sensor:4;
    unsigned pin:5;
    unsigned pinVal:12;
    float    sensorVal;
    char     rfStatus[16];

    // Set default payload values
    RF24Payload():
        cmd(15),
        sensor(15),
        pin(31),
        pinVal(1024),
        sensorVal(255.0)
        {}
};

#endif
