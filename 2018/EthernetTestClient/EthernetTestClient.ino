#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress ip(192, 168, 0, 177);
byte server[] = { 192, 168, 0, 64 };

EthernetClient client;

void setup() {
  // Open serial communication
  Serial.begin(9600);

  // Connect to ethernet using static IP
  Ethernet.begin(mac, ip);

  delay(1000);
  Serial.println("connecting...");
  
  // if you get a connection, report back via serial:
  if (client.connect(server, 502)) {
    Serial.println("connected");
    client.println("HI");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }
}

