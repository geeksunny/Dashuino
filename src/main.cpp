#include <Arduino.h>
#include <DebugLog.hpp>
#include <iomanip>

#include "pins.h"
#include <WifiTools.h>
#include "Controller.h"

lightswitch::Configuration config;
lightswitch::Controller *controller;

bool state = false;




    }
  }
void setup() {
  SETUP_SERIAL(BAUD_RATE, 3000, "Serial console ready.")
//#ifdef IS_DEBUG_MODE
//  // Preparing cout for int values as hex
//  std::cout << std::hex << std::setfill('0') << std::setw(2);
//#endif

  // Set up digital pins
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  wifi_tools::startClient();
  bool result = config.load();
  // TODO: If result is FALSE, blink LED error pattern, halt further operations
  controller = new lightswitch::Controller(config);
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