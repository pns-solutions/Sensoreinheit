/*
  ArduinoMqttClient - WiFi Simple Sender
  This example connects to a MQTT broker and publishes a message to
  a topic once a second.
  The circuit:
  - Arduino MKR 1000, MKR 1010 or Uno WiFi Rev2 board
  This example code is in the public domain.
*/

#include <ArduinoMqttClient.h>
#include "./tentacle_shield_i2c_continous_reading/tentacle_shield_i2c_continous_reading.ino"
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ESP8266_ESP12)
  #include <ESP8266WiFi.h>
#endif

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "";    // your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)

// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

//const char broker[] = "192.168.178.22";
IPAddress broker = {192,168,178,22};
int        port     = 8883;
const char topic[]  = "testTopic";

const long interval = 1000;
unsigned long previousMillis = 0;

int sensor1 = 500;
int sensor2 = 50;
int sensor3 = 60;
int sensor4 = 5;


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
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
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();

  test()
  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    Serial.println(sensor1);
    Serial.println(sensor2);
    Serial.println(sensor3);
    Serial.println(sensor4);

  
    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage("tc1/sensor1");
    mqttClient.print(sensor1);
    mqttClient.endMessage();

        // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage("tc1/sensor2");
    mqttClient.print(sensor2);
    mqttClient.endMessage();

        // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage("tc1/sensor3");
    mqttClient.print(sensor3);
    mqttClient.endMessage();

        // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage("tc1/sensor4");
    mqttClient.print(sensor4);
    mqttClient.endMessage();

    Serial.println();

  }
}
