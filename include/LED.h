#ifndef LIGHTSWITCH_INCLUDE_LED_H_
#define LIGHTSWITCH_INCLUDE_LED_H_

#include <cstdint>

namespace lightswitch {

enum class Action {
  NONE,
  ON,
  OFF,
  BLINK,
};

class LED {
  const uint8_t pin_;
  unsigned long nextTick_;
  unsigned long blinkOn_, blinkOff_;
  lightswitch::Action action_ = Action::NONE;
 public:
  LED(uint8_t pinNumber);
  virtual ~LED();
  static void loop();
  LED &setup();
  void set(bool turnedOn, unsigned long duration = 0, unsigned long delay = 0);
  void on(unsigned long durationOn = 0, unsigned long delay = 0);
  void off(unsigned long durationOff = 0, unsigned long delay = 0);
  void toggle(unsigned long duration = 0, unsigned long delay = 0);
  void blink(unsigned long durationOn, unsigned long durationOff = 0);
  bool isRunning();
  bool isOn();
 private:
  void handleLoop();
};

}

#endif //LIGHTSWITCH_INCLUDE_LED_H_
