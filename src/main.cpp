#include <Arduino.h>
#include <iostream>
#include <iomanip>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

#include "Configuration.h"
#include "Dhcp.h"
#include "LED.h"

#define LED_DURATION      500

int status = WL_IDLE_STATUS;
WiFiUDP Udp;
RIP_MSG_FIXED dhcpPacket;

lightswitch::LED *ledPrimary;
lightswitch::LED *ledSecondary;

bool state = false;

void setup() {
#ifdef DEBUG_MODE
  // Initialize serial console
  Serial.begin(BAUD_RATE);
  delay(3000);
  std::cout << "Serial console ready." << std::endl;
//  while (!Serial);  // Wait for serial port.

  // Preparing cout for int values as hex
  std::cout << std::hex << std::setfill('0') << std::setw(2);
#endif

  // Set up digital pins
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  // Set up LEDs
  ledPrimary = &(new lightswitch::LED(PIN_LED_PRIMARY))->setup();
  ledSecondary = &(new lightswitch::LED(PIN_LED_SECONDARY))->setup();

// Check for presence of wifi shield / module
  if (WiFi.status() == WL_NO_SHIELD) {
#ifdef DEBUG_MODE
    std::cout << ("WiFi shield not present") << std::endl;
#endif
    while (true); // Halt
  }

  // Wifi mode set
  WiFi.mode(WIFI_STA);

  // Attempt to connect to network
  if (WiFi.getAutoConnect()) {
#ifdef DEBUG_MODE
    std::cout << "Attempting to connect using SAVED SSID!" << std::endl;
#endif
    // Connect to saved network:
    WiFi.begin();
  } else {
#ifdef DEBUG_MODE
    std::cout << "Attempting to connect to WPA SSID: " << SSID << std::endl;
#endif
    // Connect to WPA/WPA2 network:
    WiFi.begin(SSID, PASS);
  }

  while (WiFi.status() != WL_CONNECTED) {
    // wait 0.5 seconds for connection:
    delay(500);
#ifdef DEBUG_MODE
    std::cout << ".";
#endif
  }

#ifdef DEBUG_MODE
  // Connected to network!
  std::cout << "Connected to network!" << std::endl;
#endif

  // Listen for UDP packets on port 67
  Udp.begin(DHCP_SERVER_PORT);
}

void loop() {
  // If button is pressed, light up both LEDs.
  // If not, light up an LED when UDP packet parsed.
  bool pressed = digitalRead(PIN_BUTTON) == LOW;
  if (state != pressed) {
    if (pressed) {
      ledPrimary->blink(LED_DURATION);
      ledSecondary->blinkInverted(LED_DURATION);
    } else {
      ledPrimary->off();
      ledSecondary->off();
    }
    state = pressed;
#ifdef DEBUG_MODE
    std::cout << "State changed - Button Pressed: " << (pressed ? "YES" : "NO") << std::endl;
#endif
  } else {
    // Check for UDP packets
    if (Udp.parsePacket()) {
      // Check if UDP packet is DHCP request
      if (Udp.peek() == DHCP_BOOTREQUEST && Udp.remotePort() == DHCP_CLIENT_PORT) {
        // looks like a DHCP request!
        // Light up LED_PRIMARY
        ledPrimary->on(LED_DURATION);
        // Read packet data
        parseDhcpRequest(Udp, dhcpPacket);
#ifdef DEBUG_MODE
        std::cout << "HW ADDR: "
                  << (int) dhcpPacket.chaddr[0] << ":"
                  << (int) dhcpPacket.chaddr[1] << ":"
                  << (int) dhcpPacket.chaddr[2] << ":"
                  << (int) dhcpPacket.chaddr[3] << ":"
                  << (int) dhcpPacket.chaddr[4] << ":"
                  << (int) dhcpPacket.chaddr[5] << std::endl;
#endif
      } else {
#ifdef DEBUG_MODE
        std::cout << "Skipped a UDP packet!" << std::endl;
#endif
        // Light up LED_SECONDARY
        ledSecondary->on(500);
      }
    }
  }
  // Call LED looper to process timed jobs
  lightswitch::LED::loop();
}