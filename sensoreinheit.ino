/*
  ArduinoMqttClient - WiFi Simple Sender
  This example connects to a MQTT broker and publishes a message to
  a topic once a second.
  The circuit:
  - Arduino MKR 1000, MKR 1010 or Uno WiFi Rev2 board
  This example code is in the public domain.
*/

#include <ArduinoMqttClient.h>
#include <Wire.h>                     // enable I2C.
#include "arduino_secrets.h"
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#elif defined(ARDUINO_ESP8266_ESP12)
#include <ESP8266WiFi.h>
#endif

#define TOTAL_CIRCUITS 4              // <-- CHANGE THIS | set how many I2C circuits are attached to the Tentacle shield(s): 1-8


///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

IPAddress broker = {192,168,178,22};
int        port     = 8883;

const long interval = 1000;
unsigned long previousMillis = 0;

char* channel_name;

//tentacle_shield
char sensordata[30];                  // A 30 byte character array to hold incoming data from the sensors
byte sensor_bytes_received = 0;       // We need to know how many characters bytes have been received
byte code = 0;                        // used to hold the I2C response code.
byte in_char = 0;                     // used as a 1 byte buffer to store in bound bytes from the I2C Circuit.



int channel_ids[] = {97, 98, 99, 100};// <-- CHANGE THIS.
// A list of I2C ids that you set your circuits to.
// This array should have 1-8 elements (1-8 circuits connected)

char *channel_names[] = {"DO", "ORP", "PH", "EC"}; // <-- CHANGE THIS.
// A list of channel names (must be the same order as in channel_ids[])
// it's used to give a name to each sensor ID. This array should have 1-8 elements (1-8 circuits connected).
// {"PH Tank 1", "PH Tank 2", "EC Tank 1", "EC Tank2"}, or {"PH"}



void setup() {
    //Initialize serial and wait for port to open:
    Serial.begin(9600);
    Wire.begin();                // enable I2C port.
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    // attempt to connect to WiFi network:
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
        // failed, retry
        Serial.print(".");
        delay(5000);
    }

    Serial.println("You're connected to the network");
    Serial.println();

    // You can provide a unique client ID, if not set the library uses Arduino-millis()
    // Each client must have a unique client ID
    // mqttClient.setId("clientId");

    // You can provide a username and password for authentication
    // mqttClient.setUsernamePassword("username", "password");

    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.println(broker);

    if (!mqttClient.connect(broker, port)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());

        while (1);
    }

    Serial.println("You're connected to the MQTT broker!");
    Serial.println();
}

void loop() {




    for (int channel = 0; channel < TOTAL_CIRCUITS; channel++) {       // loop through all the sensors


        // call poll() regularly to allow the library to send MQTT keep alives which
        // avoids being disconnected by the broker
        mqttClient.poll();


        Wire.beginTransmission(channel_ids[channel]);     // call the circuit by its ID number.
        Wire.write('r');                              // request a reading by sending 'r'
        Wire.endTransmission();                            // end the I2C data transmission.

        delay(1000);  // AS circuits need a 1 second before the reading is ready

        sensor_bytes_received = 0;                        // reset data counter
        memset(sensordata, 0, sizeof(sensordata));        // clear sensordata array;

        Wire.requestFrom(channel_ids[channel], 48, 1);    // call the circuit and request 48 bytes (this is more then we need).
        code = Wire.read();

        while (Wire.available()) {          // are there bytes to receive?
            in_char = Wire.read();            // receive a byte.

            if (in_char == 0) {               // null character indicates end of command
                Wire.endTransmission();         // end the I2C data transmission.
                break;                          // exit the while loop, we're done here
            }
            else {
                sensordata[sensor_bytes_received] = in_char;      // append this byte to the sensor data array.
                sensor_bytes_received++;
            }
        }

        char topic [200] = "tc1/"; 
        channel_name = channel_names[channel];
        Serial.print(channel_names[channel]);   // print channel name
        Serial.print(':');

        // TODO: das ganze ist echt zu langsam. Aktuell laufen alle
        switch (code) {                          // switch case based on what the response code is.
            case 1:                               // decimal 1  means the command was successful.
                Serial.println(sensordata);       // print the actual reading
                strcpy(topic + strlen(topic), channel_name); //TODO: Bessere lösung?

                mqttClient.beginMessage(topic);
                mqttClient.print(sensordata);
                mqttClient.endMessage();
                
                break;                                // exits the switch case.

            case 2:                                // decimal 2 means the command has failed.
                Serial.println("command failed");   // print the error
                break;                                 // exits the switch case.

            case 254:                              // decimal 254  means the command has not yet been finished calculating.
                Serial.println("circuit not ready"); // print the error
                break;                                 // exits the switch case.

            case 255:                              // decimal 255 means there is no further data to send.
                Serial.println("no data");          // print the error
                break;                                 // exits the switch case.
        }

    } // for loop









//    // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
//    // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
//    unsigned long currentMillis = millis();
//
//
//    if (currentMillis - previousMillis >= interval) {
//        // save the last time a message was sent
//        previousMillis = currentMillis;
//
//        Serial.println(sensor1);
//        Serial.println(sensor2);
//        Serial.println(sensor3);
//        Serial.println(sensor4);
//
//
//        // send message, the Print interface can be used to set the message contents
//        mqttClient.beginMessage("tc1/sensor1");
//        mqttClient.print(sensor1);
//        mqttClient.endMessage();
//
//        // send message, the Print interface can be used to set the message contents
//        mqttClient.beginMessage("tc1/sensor2");
//        mqttClient.print(sensor2);
//        mqttClient.endMessage();
//
//        // send message, the Print interface can be used to set the message contents
//        mqttClient.beginMessage("tc1/sensor3");
//        mqttClient.print(sensor3);
//        mqttClient.endMessage();
//
//        // send message, the Print interface can be used to set the message contents
//        mqttClient.beginMessage("tc1/sensor4");
//        mqttClient.print(sensor4);
//        mqttClient.endMessage();
//
//        Serial.println();
//
//    }
}
