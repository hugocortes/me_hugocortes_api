/******************************************************************************
 * @author: Hugo Cortes
 * @file: S_NRF24.cpp
 * 
 *****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// Comment/Uncomment for RPi
//#include <iostream>
//#include <RF24/RF24.h>
//#include <RF24Mesh/RF24Mesh.h>
//#include <RF24Network/RF24Network.h>
//RF24        radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

// Comment/Uncomment for Arduino
#include "RF24.h"
#include "RF24Mesh.h"
#include "RF24Network.h"
#include <SPI.h>
#define RF_CE 8
#define RF_CSN 10
RF24        radio(RF_CE, RF_CSN);
///////////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include "S_NRF24.h"
#include "RF24Payload.h"

RF24Network network(radio);
RF24Mesh    mesh(radio, network);

S_NRF24* S_NRF24::sInstance = NULL;

S_NRF24::S_NRF24(uint8_t nodeID)
{
    mesh.setNodeID(nodeID); // Set to master node
    mesh.begin();           // Connect to the mesh
}

S_NRF24* S_NRF24::instance()
{
    if(!sInstance)
    {
        printf("Initalize RF24 by calling setup(nodeID)\n");
        return NULL;
    }
    return sInstance;
}

void S_NRF24::setup(uint8_t nodeID)
/**
 * @purpose: nRF24 radio setup with given node id
 */
{
    if(!sInstance) sInstance = new S_NRF24(nodeID);
    else printf("ERROR! Already initialized!\n");
    
}

void S_NRF24::refresh()
{
    mesh.update();
    if(mesh.getNodeID() == 0) mesh.DHCP();
}

void S_NRF24::refresh(uint16_t time)
{
    unsigned long millisTimer = millis();
    while(millis() - millisTimer <= time) S_NRF24::refresh();
}

RF24Payload S_NRF24::sendRF24Payload(RF24Payload payload, uint8_t nodeID)
{
    if(!mesh.write(&payload, 'M', sizeof(payload), nodeID))
    {
        if(!mesh.checkConnection())
        {
            printf("Renewing address...\n");
            mesh.renewAddress(); 
        }
        strncpy(payload.rfStatus, "fail_send\0", sizeof("fail_send\0"));
    }
    else strncpy(payload.rfStatus, "success\0", sizeof("success\0"));
    return payload;
}

RF24Payload S_NRF24::readRF24Payload()
{
    RF24Payload payloadIn;
    unsigned long waitBegin = millis();
    bool timeout = false;
    while(!network.available())
    {
        if(signed(millis() - waitBegin) > 500)
        {
            timeout = true;
            break;
        }
    }
    if(timeout)
    {
        printf("Response timed out.\n");
        strncpy(payloadIn.rfStatus, "timeout\0", sizeof("timeout\0"));
        return payloadIn;
    }
    else
    {
        RF24NetworkHeader header;
        network.peek(header);
        switch(header.type)
        {
            case 'M':
                network.read(header,&payloadIn,sizeof(payloadIn)); 
                printf("Incoming from node: 0%o\n",header.from_node);
                break;
            default:
                printf("Rcv bad type %d from 0%o\n",header.type,header.from_node); 
                strncpy(payloadIn.rfStatus, "fail_rx\0", sizeof("fail_rx\0"));
        }
        
    }
    return payloadIn;
}

bool S_NRF24::isPacketAvailable()
{
    return network.available();
}




void S_NRF24::debugPrint()
/**
 * @purpose: Debug the current radio status by printing info.
 */
{
    radio.printDetails();
}

void S_NRF24::debugRead()
{
    // Check for incoming data from the sensors
    while(network.available()){
        //    printf("rcv\n");
        RF24NetworkHeader header;
        network.peek(header);
        uint32_t dat=0;
        network.read(header,&dat,sizeof(dat)); 
        printf("Rcv %u from 0%o\n",dat,header.from_node);
    }
}

void S_NRF24::debugSend(uint32_t data, uint8_t nodeID)
{   
    // Send an 'M' type to other Node containing the current millis()
    if(!mesh.write(&data,'M',sizeof(data), nodeID)){
        if( ! mesh.checkConnection() ){
            printf("Renewing address...\n");
            mesh.renewAddress(); 
        }else{
            printf("Send fail. Test OK\n");
        }
    }
}

void S_NRF24::debugMasterSend(uint32_t data, uint8_t nodeID)
{
    RF24NetworkHeader header;
    int16_t address = mesh.getAddress(nodeID);
    if(address != -1) header = RF24NetworkHeader(address, 'M');
    else
    {
        printf("Unknown node id!\n");
        return;  
    } 
    if( network.write(header, &data, sizeof(data) ))
    {
        printf("Send OK\n");
    }
    else
    {
        printf("Send Fail\n");
    }
}

void S_NRF24::debugPrintDetails()
{
    printf("getNodeID: %u\n", mesh.getNodeID());
}


