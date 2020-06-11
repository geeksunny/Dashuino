#include <Arduino.h>
#include <DebugLog.hpp>
#include <iomanip>

#include "LED.h"
#include "pins.h"
#include <WifiTools.h>
#include "Controller.h"

led::LED *primary;
led::LED *secondary;

lightswitch::Configuration config;
lightswitch::Controller *controller;

bool state = false;


void setup() {
  SETUP_SERIAL(BAUD_RATE, 3000, "Serial console ready.")
//#ifdef IS_DEBUG_MODE
//  // Preparing cout for int values as hex
//  std::cout << std::hex << std::setfill('0') << std::setw(2);
//#endif

  // Set up digital pins
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  primary = new led::LED{PIN_LED_PRIMARY};
  primary->setup().blink(250);
  secondary = new led::LED{PIN_LED_SECONDARY};
  secondary->setup().blink(250, 250, 250);

  wifi_tools::startClient(led::LED::loop, 250);

  if (!config.load()) {
    // Fail-state. Halt and blink LEDs.
    primary->blink(1000);
    secondary->blink(1000);
    while (true) {
      led::LED::loop();
    }
  }

  controller = new lightswitch::Controller(config, primary, secondary);
  controller->setup();
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
  controller->loop();
}