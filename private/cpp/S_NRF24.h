/**
 * @author Hugo Cortes
 * @file S_NRF24.h
 */

#ifndef S_NRF24_H
#define S_NRF24_H

#include <stdint.h>
#include "RF24Payload.h"

class S_NRF24 {
public:
    static S_NRF24* instance();
    static void setup(uint8_t nodeID);
    void refresh();
    void refresh(uint16_t time);
    bool isPacketAvailable();
    RF24Payload readRF24Payload();
    RF24Payload sendRF24Payload(RF24Payload payload, uint8_t nodeID);
    
    void debugPrint();
    void debugSend(uint32_t data, uint8_t nodeID);
    void debugMasterSend(uint32_t data, uint8_t nodeID);
    void debugRead();
private:
    S_NRF24(uint8_t nodeID);
    static S_NRF24* sInstance;
};

#endif

