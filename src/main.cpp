#include <Arduino.h>
#include <DebugLog.hpp>
#include <iomanip>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

#include <LightswitchServer.h>
#include "pins.h"
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

  void setup() {
    // Set up LEDs
    ledPrimary.setup();
    ledSecondary.setup();
  }

 private:
  bool onAction(uint8_t action, uint8_t value) override {
    DEBUG("onAction(action=", unsigned(action), ", value=", unsigned(value), ")")
    switch (action) {
      case lightswitch::LightswitchAction::TOGGLE: {
        DEBUG("action : TOGGLE")
        // Light up LED_PRIMARY, long
        ledPrimary.on(LED_DURATION_LONG);
        break;
      }
      case lightswitch::LightswitchAction::SET_COLOR: {
        DEBUG("action : SET_COLOR")
        // Light up LED_SECONDARY, long
        ledSecondary.on(LED_DURATION_LONG);
        break;
      }
      case lightswitch::LightswitchAction::SET_SCENE: {
        DEBUG("action : SET_SCENE")
        // Light up LED_PRIMARY, short
        ledPrimary.on(LED_DURATION_SHORT);
        break;
      }
      case lightswitch::LightswitchAction::BRIGHTNESS: {
        DEBUG("action : BRIGHTNESS")
        // Light up LED_SECONDARY, short
        ledSecondary.on(LED_DURATION_SHORT);
        break;
      }
      case lightswitch::LightswitchAction::CYCLE: {
        DEBUG("action : CYCLE")
        ledPrimary.blink(LED_DURATION_SHORT);
        ledSecondary.blinkInverted(LED_DURATION_SHORT);
        break;
      }
      default: {
        DEBUG("action : default...")
        // Nothing to do.
        break;
      }
    }
    return false;
  }

};

Handler *handler;
lightswitch::LightswitchServer *server;

bool state = false;

void setup() {
  SETUP_SERIAL(BAUD_RATE, 3000, "Serial console ready.")
#ifdef DEBUG_MODE
  // Preparing cout for int values as hex
  std::cout << std::hex << std::setfill('0') << std::setw(2);
#endif

  // Set up digital pins
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  wifi_tools::startClient();
  handler = new Handler();
  handler->setup();
  server = new lightswitch::LightswitchServer(*handler);
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
    DEBUG("State changed - Button Pressed: ", (pressed ? "YES" : "NO"))
  }
  // Process server loop
  server->loop();
  // Call LED looper to process timed jobs
  lightswitch::LED::loop();
}