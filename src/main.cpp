#include <Arduino.h>
#include <iostream>
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
    std::cout << ("WiFi shield not present") << std::endl;
#endif
    while (true); // Halt
  }

  // Attempt to connect to network
  while (status != WL_CONNECTED) {
#ifdef DEBUG_MODE
    std::cout << "Attempting to connect to WPA SSID: " << SSID << std::endl;
#endif
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(SSID, PASS);

    // wait 10 seconds for connection:
    delay(10000);
  }

#ifdef DEBUG_MODE
  // Connected to network!
  std::cout << "Connected to network!" << std::endl;
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
      std::cout << "Packet Data: " << std::endl << buf << std::endl << std::endl;
#endif
    } else {
#ifdef DEBUG_MODE
      std::cout << "Skipped a UDP packet!" << std::endl;
#endif
    }
  }
}