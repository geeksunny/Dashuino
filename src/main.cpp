#include <Arduino.h>
#include <iostream>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

#include "Configuration.h"
#include "LED.h"

#define UDP_PORT_LOCAL    67
#define UDP_PORT_REMOTE   68
#define LED_DURATION      500

int status = WL_IDLE_STATUS;
WiFiUDP Udp;

lightswitch::LED ledPrimary(PIN_LED_PRIMARY);
lightswitch::LED ledSecondary(PIN_LED_SECONDARY);

bool state = false;

void setup() {
#ifdef DEBUG_MODE
  // Initialize serial console
  Serial.begin(BAUD_RATE);
  delay(3000);
  std::cout << "Serial console ready." << std::endl;
//  while (!Serial);  // Wait for serial port.
#endif

  // Set up digital pins
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  // Set up LEDs
  ledPrimary.setup();
  ledSecondary.setup();

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
  // If button is pressed, light up both LEDs.
  // If not, light up an LED when UDP packet parsed.
  bool pressed = digitalRead(PIN_BUTTON) == LOW;
  if (state != pressed) {
    if (pressed) {
      ledPrimary.blink(LED_DURATION);
      ledSecondary.blinkInverted(LED_DURATION);
    } else {
      ledPrimary.off();
      ledSecondary.off();
    }
    state = pressed;
#ifdef DEBUG_MODE
    std::cout << "State changed - Button Pressed: " << (pressed ? "YES" : "NO") << std::endl;
#endif
  } else {
    // Check for UDP packets
    if (Udp.parsePacket()) {
      if (Udp.remotePort() == UDP_PORT_REMOTE) {
        // looks like a DHCP request!
        // TODO: Should we verify source IP is 0.0.0.0 ?
        // Light up LED_PRIMARY
        ledPrimary.on(LED_DURATION);
        // Read packet data
        char buf[UDP_TX_PACKET_MAX_SIZE];
        Udp.read(buf, UDP_TX_PACKET_MAX_SIZE);
#ifdef DEBUG_MODE
        std::cout << "Packet Data: " << std::endl << buf << std::endl << std::endl;
#endif
      } else {
#ifdef DEBUG_MODE
        std::cout << "Skipped a UDP packet!" << std::endl;
#endif
        // Light up LED_SECONDARY
        ledSecondary.on(500);
      }
    }
  }
  // Call LED looper to process timed jobs
  lightswitch::LED::loop();
}