#include <Arduino.h>
#include <iostream>
#include <iomanip>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

#include "Configuration.h"
#include <LightswitchServer.h>
#include <WifiTools.h>
#include "LED.h"

#define LED_DURATION_LONG      2500
#define LED_DURATION_SHORT     500

class Handler : public lightswitch::ActionHandler {

 public:
  lightswitch::LED ledPrimary;
  lightswitch::LED ledSecondary;

  Handler() : ledPrimary{PIN_LED_PRIMARY}, ledSecondary{PIN_LED_SECONDARY} {
    //
  }

 private:
  void setup() {
    // Set up LEDs
    ledPrimary.setup();
    ledSecondary.setup();
  }

  bool onAction(uint8_t action, uint8_t value) override {
    switch (action) {
      case lightswitch::Action::TOGGLE: {
        // Light up LED_PRIMARY, long
        ledPrimary.on(LED_DURATION_LONG);
        break;
      }
      case lightswitch::Action::SET_COLOR: {
        // Light up LED_SECONDARY, long
        ledSecondary.on(LED_DURATION_LONG);
        break;
      }
      case lightswitch::Action::SET_SCENE: {
        // Light up LED_PRIMARY, short
        ledPrimary.on(LED_DURATION_SHORT);
        break;
      }
      case lightswitch::Action::BRIGHTNESS: {
        // Light up LED_SECONDARY, short
        ledSecondary.on(LED_DURATION_SHORT);
        break;
      }
      case lightswitch::Action::CYCLE: {
        ledPrimary.blink(LED_DURATION_SHORT);
        ledSecondary.blinkInverted(LED_DURATION_SHORT);
        break;
      }
      default: {
        // Nothing to do.
        break;
      }
    }
    return false;
  }

};

std::shared_ptr<Handler> handler;
std::shared_ptr<lightswitch::LightswitchServer> server;

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

  wifi_tools::startClient();
  handler = std::make_shared<Handler>();
  server = std::make_shared<lightswitch::LightswitchServer>(*handler);
  server->setup();
}

void loop() {
  // If button is pressed, light up both LEDs.
  // If not, light up an LED when UDP packet parsed.
  bool pressed = digitalRead(PIN_BUTTON) == LOW;
  if (state != pressed) {
    if (pressed) {
      handler->ledPrimary.blink(LED_DURATION_LONG);
      handler->ledSecondary.blinkInverted(LED_DURATION_LONG);
    } else {
      handler->ledPrimary.stop();
      handler->ledSecondary.stop();
    }
    state = pressed;
#ifdef DEBUG_MODE
    std::cout << "State changed - Button Pressed: " << (pressed ? "YES" : "NO") << std::endl;
#endif
  }
  // Process server loop
  server->loop();
  // Call LED looper to process timed jobs
  lightswitch::LED::loop();
}