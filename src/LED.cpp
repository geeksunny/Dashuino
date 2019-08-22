#include <Arduino.h>
#include <deque>
#include "LED.h"

namespace lightswitch {

std::deque<LED *> leds;

LED::LED(uint8_t pinNumber) : pin_(pinNumber) {
  leds.push_back(this);
}

LED::~LED() {
  for (auto it = leds.begin(); it != leds.end(); ++it) {
    if (*it == this) {
      leds.erase(it);
      return;
    }
  }
}

void LED::loop() {
  for (auto &led : leds) {
    // TODO: Should there be a flag check to ensure ::setup() was called?
    led->handleLoop();
  }
}

LED &LED::setup() {
  pinMode(pin_, OUTPUT);
  return *this;
}

void LED::set(bool turnedOn, unsigned long duration, unsigned long delay) {
  if (duration == 0) {
    digitalWrite(pin_, HIGH);
  } else {
    nextTick_ = millis() + duration;
    action_ = turnedOn ? Action::ON : Action::OFF;
  }
}

void LED::on(const unsigned long durationOn, const unsigned long delay) {
  set(true, durationOn, delay);
}

void LED::off(const unsigned long durationOff, const unsigned long delay) {
  set(false, durationOff, delay);
}

void LED::toggle(const unsigned long duration, const unsigned long delay) {
  set((digitalRead(pin_) == LOW), duration, delay);
}

void LED::blink(const unsigned long durationOn, const unsigned long durationOff) {
  on();
  if (durationOn > 0) {
    blinkOn_ = durationOn;
    blinkOff_ = (durationOff == 0) ? durationOn : durationOff;
    nextTick_ = millis() + durationOn;
    action_ = Action::BLINK;
  }
}

bool LED::isRunning() {
  return action_ != Action::NONE;
}

bool LED::isOn() {
  return digitalRead(pin_) == HIGH;
}

void LED::handleLoop() {
  if (action_ == Action::NONE) {
    return;
  }
  if (millis() >= nextTick_) {
    switch (action_) {
      case Action::ON: {
        on();
        action_ = Action::NONE;
        break;
      }
      case Action::OFF: {
        off();
        action_ = Action::NONE;
        break;
      }
      case Action::BLINK: {
        bool litUp = isOn();
        nextTick_ = litUp ? blinkOff_ : blinkOn_;
        set(!litUp);
        break;
      }
      case Action::NONE: {
        // Shouldn't be here
      }
    }
  }
}

}
