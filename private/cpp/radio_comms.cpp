/******************************************************************************
 * @author: Hugo Cortes
 * @file: mesh_example_rpi.cpp
 * 
 *****************************************************************************/

#include <fstream>
#include <iostream>
#include <stddef.h>
#include <unistd.h>
#include "S_NRF24.h"
#include "RF24Payload.h"
#include "../libs/rapidjson/writer.h"
#include "../libs/rapidjson/stringbuffer.h"
using namespace rapidjson;

// Function Prototypes
void welcomeMessage(int argc, char* argv[]);
void jsonWrite(std::string jsonPacket);
void writePacketToJson(RF24Payload payloadIn, char api_call[], char id[]);
void writeJsonGetSensor(RF24Payload payloadIn);
void writeJsonGetPin(RF24Payload payloadIn);
void writeJsonFail(RF24Payload packet);
RF24Payload getSensorPayload(char* sensor);
RF24Payload getGetPinPayload(char* pin);
RF24Payload getPutPinPayload(char* pin, char* value);

// Constants
// API CALL VALUES
const uint8_t GET_PIN    = 1;
const uint8_t PUT_PIN    = 2;
const uint8_t GET_SENSOR = 3;

// API Call Strings
const char API_GET_SENSOR[] = {"get_sensor"};
const char API_GET_PIN[]    = {"get_pin"};
const char API_PUT_PIN[]    = {"put_pin"};

// SENSOR VALUES
const uint8_t TEMP_SENS  = 1;
const uint8_t HUMID_SENS = 2;
const uint8_t HEAT_SENS  = 3;

// NODE IDs
const uint8_t MASTER_ID = 0;
const uint8_t F1_ID     = 1;
const uint8_t F2_ID     = 2;

// NODE STRING IDs
const char F1[] = {"f1"};
const char F2[] = {"f2"};

// CLI arguments
const uint8_t API_CALL = 1;
const uint8_t ID = 2;
const uint8_t PIN = 3;
const uint8_t SENSOR = 3;
const uint8_t VALUE = 4;
const uint8_t FROM = 4;

// SET UP JSON
StringBuffer json;
Writer<StringBuffer> writer(json);
const char RADIO_PACKET[] = {"packet/radio_packet.json"};

int main(int argc, char* argv[])
{
    welcomeMessage(argc, argv);

    if(argc != 4 && argc != 5)
    {
        printf("Usage: sudo ./program {api call} {id} {pin|sensor} {value|from}\n");
        return 1;
    }    
    S_NRF24::setup(MASTER_ID);
    S_NRF24::instance()->refresh(750);
    writer.StartObject();

    RF24Payload payloadOut;
    RF24Payload payloadIn;
    if     (!strcmp(argv[API_CALL], API_GET_SENSOR)) payloadOut = getSensorPayload(argv[3]);
    else if(!strcmp(argv[API_CALL], API_GET_PIN))    payloadOut = getGetPinPayload(argv[3]);
    else if(!strcmp(argv[API_CALL], API_PUT_PIN))    payloadOut = getPutPinPayload(argv[3], argv[4]);
    else strncpy(payloadOut.rfStatus, "unkwn_call\0", sizeof("unkwn_call\0"));

    if(!strcmp(payloadOut.rfStatus, "master"))
    {
        // Sending
        if(!strcmp(argv[ID], F1))
        {
            payloadOut = S_NRF24::instance()->sendRF24Payload(payloadOut, F1_ID);
        }
        else if(!strcmp(argv[ID], F2))
        {
            payloadOut = S_NRF24::instance()->sendRF24Payload(payloadOut, F2_ID);
        }
        else strncpy(payloadOut.rfStatus, "unkwn_id\0", sizeof("unkwn_id\0"));
        
        // Receiving
        if(!strcmp(payloadOut.rfStatus, "success"))
        {
            S_NRF24::instance()->refresh(1000);
            payloadIn = S_NRF24::instance()->readRF24Payload();
            writePacketToJson(payloadIn, argv[API_CALL], argv[ID]);
        }
        else writeJsonFail(payloadOut);
    }
    else writeJsonFail(payloadOut);

    printf("Finished MasterRFComms.cpp\n");
    printf("-----------------------------------------------\n");
    return 0;
}

void welcomeMessage(int argc, char* argv[])
{
    printf("-----------------------------------------------\n");
    printf("Executing MasterRFComms.cpp\n");
    printf("argc = %d : ", argc);
    for (int i =0; i<argc; i++) printf("argv[%i] = %s ", i, argv[i]);
    printf("\n");
}

RF24Payload getSensorPayload(char* sensor)
{
    RF24Payload payload;
    strncpy(payload.rfStatus, "master\0", sizeof("master\0"));
    payload.cmd = GET_SENSOR;
    if     (!strcmp(sensor, "temp"))     payload.sensor = TEMP_SENS;
    else if(!strcmp(sensor, "heat"))     payload.sensor = HEAT_SENS;
    else if(!strcmp(sensor, "humidity")) payload.sensor = HUMID_SENS;
    else     strncpy(payload.rfStatus, "unk_sensor\0", sizeof("unk_sensor\0"));
    return payload;
}

RF24Payload getGetPinPayload(char* pin)
{
    RF24Payload payload;
    strncpy(payload.rfStatus, "master\0", sizeof("master\0"));
    payload.cmd = GET_PIN;
    payload.pin = std::stoi(pin);
    return payload;
}

RF24Payload getPutPinPayload(char* pin, char* value)
{
    RF24Payload payload;
    strncpy(payload.rfStatus, "master\0", sizeof("master\0"));
    payload.cmd    = PUT_PIN;
    payload.pin    = std::stoi(pin);
    payload.pinVal = std::stoi(value);
    return payload;
}

void writePacketToJson(RF24Payload payloadIn, char api_call[], char id[])
{
    writer.Key("status");
    if(!strcmp(payloadIn.rfStatus, "success"))
    {
        writer.String(payloadIn.rfStatus);
        writer.Key("id");
        writer.String(id);
        if     (!strcmp(api_call, API_GET_SENSOR)) writeJsonGetSensor(payloadIn);
        else if(!strcmp(api_call, API_GET_PIN))    writeJsonGetPin(payloadIn);
    }
    else writer.String(payloadIn.rfStatus);

    writer.EndObject();
    jsonWrite(json.GetString());
    printf("%s\n", json.GetString());
}

void writeJsonGetSensor(RF24Payload payloadIn)
{
    writer.Key("sensor");
    if     (payloadIn.sensor == TEMP_SENS)  writer.String("temperature");
    else if(payloadIn.sensor == HUMID_SENS) writer.String("humidity");
    else if(payloadIn.sensor == HEAT_SENS)  writer.String("heat_index");
    writer.Key("sensor_value");
    writer.Double(payloadIn.sensorVal);
}

void writeJsonGetPin(RF24Payload payloadIn)
{
    writer.Key("pin");
    writer.String(std::to_string((int)payloadIn.pin).c_str());
    writer.Key("pin_value");
    writer.String(std::to_string((int)payloadIn.pinVal).c_str());
}

void writeJsonFail(RF24Payload packet)
{
    writer.Key("status");
    writer.String(packet.rfStatus);
    writer.EndObject();
    jsonWrite(json.GetString());
    printf("%s\n", json.GetString());
}

void jsonWrite(std::string jsonPacket)
{
    std::ofstream jsonFile;
    jsonFile.open(RADIO_PACKET);
    jsonFile << jsonPacket;
    jsonFile.close();
}

