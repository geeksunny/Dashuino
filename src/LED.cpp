#include <Arduino.h>
#include <deque>
#include "LED.h"

namespace lightswitch {

std::deque<LED *> leds;

LED::LED(const uint8_t pinNumber) : pin_(pinNumber) {
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
  if (leds.empty()) {
    return;
  }
  unsigned long now = millis();
  for (auto &led : leds) {
    // TODO: Should there be a flag check to ensure ::setup() was called?
    if (led->action_ == Action::NONE) {
      return;
    }
    if (now >= led->nextTick_) {
      if (led->delay_) {
        led->handleDelay();
      } else {
        led->handleAction();
      }
    }
  }
}

LED &LED::setup() {
  pinMode(pin_, OUTPUT);
  return *this;
}

void LED::set(const bool turnedOn, const unsigned long duration, const unsigned long delay) {
  if (duration == 0 && delay == 0) {
    digitalWrite(pin_, HIGH);
  } else if (delay > 0) {
    delay_ = true;
    nextTick_ = millis() + delay;
    if (turnedOn) {
      timeOn_ = duration;
    } else {
      timeOff_ = duration;
    }
  } else {
    delay_ = false;
    nextTick_ = millis() + duration;
  }
  action_ = turnedOn ? Action::ON : Action::OFF;
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

void LED::blink(const unsigned long durationOn, const unsigned long durationOff, const unsigned long delay) {
  if (delay == 0) {
    on();
  }
  if (durationOn > 0) {
    timeOn_ = durationOn;
    timeOff_ = (durationOff == 0) ? durationOn : durationOff;
    if (delay > 0) {
      delay_ = true;
      nextTick_ = millis() + delay;
    } else {
      nextTick_ = millis() + durationOn;
    }
    action_ = Action::BLINK;
  }
}

void LED::stop() {
  action_ = Action::NONE;
}

bool LED::isRunning() {
  return action_ != Action::NONE;
}

bool LED::isOn() {
  return digitalRead(pin_) == HIGH;
}

void LED::handleDelay() {
  delay_ = false;
  switch (action_) {
    case Action::ON: {
      on();
      if (timeOn_ > 0) {
        nextTick_ = millis() + timeOn_;
      }
      break;
    }
    case Action::OFF: {
      off();
      if (timeOff_ > 0) {
        nextTick_ = millis() + timeOff_;
      }
      break;
    }
    case Action::BLINK: {
      bool setTo = !isOn();
      set(setTo);
      nextTick_ = millis() + ((setTo) ? timeOn_ : timeOff_);
      break;
    }
    case Action::NONE: {
      // Shouldn't be here
    }
  }
}

void LED::handleAction() {
  switch (action_) {
    case Action::ON: {
      off();
      stop();
      break;
    }
    case Action::OFF: {
      on();
      stop();
      break;
    }
    case Action::BLINK: {
      bool litUp = isOn();
      nextTick_ = litUp ? timeOff_ : timeOn_;
      set(!litUp);
      break;
    }
    case Action::NONE: {
      // Shouldn't be here
    }
  }
}

}