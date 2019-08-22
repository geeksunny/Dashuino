#ifndef LIGHTSWITCH_INCLUDE_CONFIGURATION_H_
#define LIGHTSWITCH_INCLUDE_CONFIGURATION_H_

#include <pins_arduino.h>

// Uncomment to enable debug output via Serial console.
#define DEBUG_MODE


//// Debug Output ////
#define BAUD_RATE             115200


//// DIGITAL PINS ////
#define PIN_BUTTON            D5
#define PIN_LED_PRIMARY       D6
#define PIN_LED_SECONDARY     D7


//// Wifi Client ////
#define SSID "NetworkName"
#define PASS "NetworkPassword"


#endif //LIGHTSWITCH_INCLUDE_CONFIGURATION_H_
