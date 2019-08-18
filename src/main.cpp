#include <Arduino.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

#include "Configuration.h"

#define UDP_PORT_LOCAL    67
#define UDP_PORT_REMOTE   68

int status = WL_IDLE_STATUS;
WiFiUDP Udp;

void setup() {
#ifdef DEBUG_MODE
  // Initialize serial console
  Serial.begin(BAUD_RATE);
//  while (!Serial);  // Wait for serial port.
#endif

  // Check for presence of wifi shield / module
  if (WiFi.status() == WL_NO_SHIELD) {
#ifdef DEBUG_MODE
    Serial.println("WiFi shield not present");
#endif
    while (true); // Halt
  }

  // Attempt to connect to network
  while (status != WL_CONNECTED) {
#ifdef DEBUG_MODE
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(SSID);
#endif
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(SSID, PASS);

    // wait 10 seconds for connection:
    delay(10000);
  }

#ifdef DEBUG_MODE
  // Connected to network!
  Serial.println("Connected to network!");
#endif

  // Listen for UDP packets on port 67(/68?)
  Udp.begin(UDP_PORT_LOCAL);
}

void loop() {
  // Check for UDP packets
  if (Udp.parsePacket()) {
    if (Udp.remotePort() == UDP_PORT_REMOTE) {
      // looks like a DHCP request!
      // TODO: Should we verify source IP is 0.0.0.0 ?
      char buf[UDP_TX_PACKET_MAX_SIZE];
      Udp.read(buf, UDP_TX_PACKET_MAX_SIZE);
#ifdef DEBUG_MODE
      Serial.println("Packet Data: ");
      Serial.println(buf);
      Serial.println();
#endif
    } else {
#ifdef DEBUG_MODE
      Serial.println("Skipped a UDP packet!");
#endif
    }
  }
}