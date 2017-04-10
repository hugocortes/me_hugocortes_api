# Doc for api.hugocortes.me 
Simple API using nRF24 transceivers

# Installations  
### MySQL  
apt-get install libmysqlclient-dev  
### NodeJS  
curl -sL https://deb.nodesource.com/setup_7.x | sudo -E bash -  
sudo apt-get install -y nodejs  
### Installing tmrh20's RF24 library (Using SPIDEV)  
https://tmrh20.github.io/RF24/RPi.html  
### Install BCM2835 library  
### Download nrf24 github libraries  
https://github.com/TMRh20  

# API Link
The base URL for all API calls is: [http://www.api.hugocortes.me/](http:///www.api.hugocortes.me/)

Using this link, appending the approriate sub-url will access the API.

# **GET** Calls

## Raspberry Pi Specific
`/rpi/gpio/{id}`  
**Inputs:** {id}: Specifies the gpio pin to read from Raspberry Pi.  
**Outputs:** GPIO pin value.

`/rpi/gpio/`  
**Inputs:** None  
**Outputs:** All the Raspberry Pi's GPIO pin values.

## Arduino Specific
`/radio/sensor/`
UPDATE: Update and remove. Left over from testing

`/radio/f{arduino_id}/sensor/{sens}`  
**Inputs:**
* {arduino_id}: Specifies which arduino to access. *Only 1 Arduino active currently*
* {sens}: Specifies which sensor to access. **temp, humidity, heat**

**Outputs:**  
Output is in the form of a JSON packet in the following format:  
`{"status":"OUTPUT","id":"OUTPUT","sensor":"OUTPUT","sensor_val":"OUTPUT"}`

`/radio/f{arduino_id}/pin/{pin}`  
**Inputs:**
* {arduino_id}: Specifies which arduino to access. *Only 1 Arduino active currently*
* {pin}: Specifies which arduino pin to access.

**Outputs:**  
Output is in the form of a JSON packet in the following format:  
`{"status":"OUTPUT","id":"OUTPUT","pin":"OUTPUT","pin_val":"OUTPUT"}`

# **PUT** Calls

## Arduino Specific
`/radio/f{arduino_id}/pin/{pin}`  
**Inputs:**
* {arduino_id}: Specifies which arduino to access. *Only 1 Arduino active currently*
* {pin}: Specifies which arduino pin to access.
* JSON packet is also expected with the Arduino value to be written to pin.
