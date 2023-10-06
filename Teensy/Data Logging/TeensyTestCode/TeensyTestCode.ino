#include <NativeEthernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; 
IPAddress ip(192, 168, 1, 177);
IPAddress laptopIP(192, 168, 1, 100);
unsigned int port = 1234;

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

void loop() {
  if (client.connected()) {
    int randomValue = random(0, 10000); // Generate random number between 0 and 9999
    client.println(randomValue); // Send random data
  } else {
    Serial.println("Disconnected. Attempting to reconnect...");
    client.connect(laptopIP, port);
    delay(500);
  }
}
