#include <NativeEthernet.h>
#include <ArduinoJson.h>
#include <vector>
#include "SharedVariables.h"

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; 
IPAddress ip(192, 168, 1, 177);
IPAddress laptopIP(192, 168, 1, 100);
unsigned int port = 1234;
EthernetServer server(80);
EthernetClient client;

void setup() {
  Serial.begin(9600);
  Serial.println("Beginning Ethernet Connection");
  Ethernet.begin(mac, ip);
  delay(1000);
  Serial.println("Connecting...");

  if (client.connect(laptopIP, port)) {
    Serial.println("Connected to the laptop!");
  } else {
    Serial.println("Connection failed");
  }
}

void addThermocoupleData(std::vector<SensorData*>& sensors) {
  ThermocoupleData* thermo = new ThermocoupleData;
  thermo->temperatures.push_back(25.0);
  thermo->temperatures.push_back(26.0);
  sensors.push_back(thermo);
}

// You can add other similar functions for other sensor types

void loop() {
  std::vector<SensorData*> sensors;

  // Add sensor data
  addThermocoupleData(sensors);
  // Add other types of sensors in similar fashion
  
  client = server.available();
  if (client) {
    DynamicJsonDocument doc(2048);
    JsonArray sensorArray = doc.createNestedArray("sensors");

    for (SensorData* sensor : sensors) {
      JsonObject sensorJson = sensorArray.createNestedObject();
      sensor->addToJson(sensorJson);
    }
    
    serializeJson(doc, client);
    client.println();

    // Free dynamically allocated memory
    for (SensorData* sensor : sensors) {
      delete sensor;
    }
    sensors.clear();
  }
}