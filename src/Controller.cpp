#include "Controller.h"

namespace lightswitch {

////////////////////////////////////////////////////////////////
// Class : Defaulter ///////////////////////////////////////////
////////////////////////////////////////////////////////////////

Defaulter::Defaulter(DefaulterConfig &config) : config_(config) {
  //
}

Defaulter::operator bool() {
  return config_.enabled_;
}

Defaulter::operator bool() const {
  return config_.enabled_;
}

void Defaulter::loop(sphue::Sphue &api) {
  unsigned long now = millis();
  if (now >= nextPollTime_) {
    auto response = api.getAllLights();
    if (response) {
      // Review list of Lights returned from API
      for (auto it = (**response).begin(); it != (**response).end(); ++it) {
        // TODO: check if *it is in lights_; create or update accordingly
      }
    }
    nextPollTime_ = now + config_.refresh_rate_;
  }
}

////////////////////////////////////////////////////////////////
// Class : Controller //////////////////////////////////////////
////////////////////////////////////////////////////////////////

Controller::Controller(Configuration &configuration)
    : server_(*this),
      config_(configuration),
      sphue_(create_sphue()),
      defaulter_(configuration.defaulter_config_),
      cycler_(std::bind(&Controller::exec_color, this, std::placeholders::_1, std::placeholders::_2)) {
  //
}

void Controller::loop() {
  server_.loop();
  cycler_.task();
  if (defaulter_) {
    defaulter_.loop(sphue_);
  }
}

void Controller::exec_color(uint8_t cycle_id, color::Hsv32 *color) {
  // TODO: assign color to specified target via cycle_id
}

sphue::Sphue Controller::create_sphue() {
  // TODO: Check for stored server address!
  //  Call `autoDiscoverHub()` only when server address is not stored.
  sphue::Sphue result = sphue::autoDiscoverHub();
  if (result) {
    // TODO: store newly discovered server address
  }
  return result;
}

bool Controller::onAction(uint8_t action, uint8_t value) {
  // TODO: handle action, dispatch commands to sphue object
  return false;
}

}