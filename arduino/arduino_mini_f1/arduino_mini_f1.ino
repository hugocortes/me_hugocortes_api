/**
 * @author Hugo Cortes
 * @file arduino_mini_f1.ino
 */

#include <printf.h>
#include "DHT.h"
#include "S_NRF24.h"
#include "RF24Payload.h"

// DEFINE PINS
#define DHT_DATA 9
#define DHT_TYPE DHT22 // AM2302 temp sensor
#define ON 1
#define OFF 0

// API CALL VALUES
const uint8_t GET_PIN    = 1;
const uint8_t PUT_PIN    = 2;
const uint8_t GET_SENSOR = 3;

// API Call Strings
const char* API_GET_SENSOR[] = {"getSensor"};
const char* API_GET_PIN[]    = {"getPin"};
const char* API_PUT_PIN[]    = {"putPin"};

// SENSOR VALUES
const uint8_t TEMP_SENS  = 1;
const uint8_t HUMID_SENS = 2;
const uint8_t HEAT_SENS  = 3;

// NODE IDs
const uint8_t MASTER_ID = 0;
const uint8_t F1_ID     = 1;
const uint8_t F2_ID     = 2;

// NODE STRING IDs
const char* F1[] = {"f1"};

DHT dht(DHT_DATA, DHT_TYPE);

void setup()
{
    Serial.begin(115200);
    printf_begin();
    printf("Executing...\n");
    S_NRF24::setup(F1_ID);
    S_NRF24::instance()->debugPrint();
    setupPins();
}

void loop()
{
    S_NRF24::instance()->refresh(100);
    if (S_NRF24::instance()->isPacketAvailable())
    {
        RF24Payload payloadIn;
        payloadIn = S_NRF24::instance()->readRF24Payload();
        switch(payloadIn.cmd){
          case GET_PIN:
              printf("In getPin!\n");
              sendGetDigitalPin(payloadIn);
              break;
          case PUT_PIN:
              printf("In putPin!\n");
              sendPutDigitalPin(payloadIn);
              break;
          case GET_SENSOR:
              printf("In getSensor!\n");
              sendGetSensor(payloadIn);
              break;
          default:
              RF24Payload payloadOut;
              strncpy(payloadOut.rfStatus, "invalidCmd\0", sizeof("invalidCmd\0"));
              S_NRF24::instance()->sendRF24Payload(payloadOut, MASTER_ID);
              break;
        }
    }
}

void sendGetSensor(RF24Payload payloadIn)
{
    RF24Payload payloadOut;
    bool read = false;
    float data = NAN;
    float f = NAN;
    float h = NAN;
    switch(payloadIn.sensor)
    {
        case TEMP_SENS:
            //for(int _ = 0; _ < 3; _++) data = dht.readTemperature(true);
            data = dht.readTemperature(true);
            break;
        case HUMID_SENS:
            //for(int _ = 0; _ < 3; _++) data = dht.readHumidity();
            data = dht.readHumidity();
            break;
        case HEAT_SENS:
//            for(int _ = 0; _ < 3; _++)
//            {
//                h = dht.readHumidity();
//                f = dht.readTemperature(true);
//            }
            h = dht.readHumidity();
            f = dht.readTemperature(true);
            if (!isnan(h) && !isnan(f)) data = dht.computeHeatIndex(f,h);
            break;
        default:
            strncpy(payloadOut.rfStatus, "invalidSen\0", sizeof("invalidSen\0"));
            S_NRF24::instance()->sendRF24Payload(payloadOut, MASTER_ID);
            return;
    }
    if (isnan(data)) 
    {
        strncpy(payloadOut.rfStatus, "sensErr\0", sizeof("sensErr\0"));
    }
    else
    {
        strncpy(payloadOut.rfStatus, "success\0", sizeof("success\0"));
        payloadOut.cmd = payloadIn.cmd;
        payloadOut.sensor = payloadIn.sensor;
        payloadOut.sensorVal = data;
    }
    S_NRF24::instance()->sendRF24Payload(payloadOut, MASTER_ID);
}

void sendGetDigitalPin(RF24Payload payloadIn)
{
    RF24Payload payloadOut;
    switch(payloadIn.pin)
    {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 9:
            strncpy(payloadOut.rfStatus, "success\0", sizeof("success\0"));
            payloadOut.cmd = payloadIn.cmd;
            payloadOut.pin = payloadIn.pin;
            payloadOut.pinVal = digitalRead(payloadIn.pin);
            break;
        default:
            strncpy(payloadOut.rfStatus, "invalidPin\0", sizeof("invalidPin\0"));
    }
    S_NRF24::instance()->sendRF24Payload(payloadOut, MASTER_ID);
}

void sendPutDigitalPin(RF24Payload payloadIn)
{
    RF24Payload payloadOut;
    switch(payloadIn.pin)
    {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 9:
            digitalWrite(payloadIn.pin, payloadIn.pinVal > 0);
            payloadOut.cmd = payloadIn.cmd;
            strncpy(payloadOut.rfStatus, "success\0", sizeof("success\0"));
            break;
        default:
            strncpy(payloadOut.rfStatus, "invalidPin\0", sizeof("invalidPin\0"));
    }
    S_NRF24::instance()->sendRF24Payload(payloadOut, MASTER_ID);    
}

void sendGetAnalogPin()
{

}

void sendPutAnalogPin()
{

}

void sendSetPinIO()
{

}

void setupPins(void){
    // SETTING UP AS RANDOM PINS FOR NOW.
    // Future use will be for relays
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(9, OUTPUT);
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    digitalWrite(4, HIGH);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(9, LOW);
}
