#ifndef LIGHTSWITCH_INCLUDE_LED_H_
#define LIGHTSWITCH_INCLUDE_LED_H_

#include <cstdint>

namespace lightswitch {

enum class LEDAction {
  NONE,
  ON,
  OFF,
  BLINK
};

// TODO: Refactor to more of a builder pattern?
class LED {
  const uint8_t pin_;
  bool delay_;
  unsigned long nextTick_;
  unsigned long timeOn_, timeOff_;
  lightswitch::LEDAction action_ = LEDAction::NONE;
 public:
  LED(uint8_t pinNumber);
  virtual ~LED();
  static void loop();
  LED &setup();
  void set(bool turnedOn, unsigned long duration = 0, unsigned long delay = 0);
  void on(unsigned long durationOn = 0, unsigned long delay = 0);
  void off(unsigned long durationOff = 0, unsigned long delay = 0);
  void toggle(unsigned long duration = 0, unsigned long delay = 0);
  void blink(unsigned long durationOn, unsigned long durationOff = 0, unsigned long delay = 0);
  void blinkInverted(unsigned long durationOn, unsigned long durationOff = 0, unsigned long delay = 0);
  void stop();
  bool isRunning();
  bool isOn();
 private:
  void blink(bool startTurnedOn, unsigned long durationOn, unsigned long durationOff, unsigned long delay);
  void handleDelay();
  void handleAction();
};

}

#endif //LIGHTSWITCH_INCLUDE_LED_H_
