#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "ssid.h"

#define UDP_PORT_LOCAL    67
#define UDP_PORT_REMOTE   68

int status = WL_IDLE_STATUS;
WiFiUDP Udp;

void setup() {
  // Initialize serial console
  Serial.begin(9600);
//  while (!Serial);  // Wait for serial port.

  // Check for presence of wifi shield / module
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true); // Halt
  }

  Serial.print("WiFi firmware version: ");
  Serial.println(WiFi.firmwareVersion());

  // Attempt to connect to network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(SSID);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(SSID, PASS);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // Connected to network!
  Serial.println("Connected to network!");

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
      Serial.println("Packet Data: ");
      Serial.println(buf);
      Serial.println();
    } else {
      Serial.println("Skipped a UDP packet!");
    }
  }
}