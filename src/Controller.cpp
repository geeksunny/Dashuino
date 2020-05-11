#include "Controller.h"

#define LIGHT_CT_DEFAULT_VALUE 366

namespace lightswitch {

////////////////////////////////////////////////////////////////
// Class : Defaulter ///////////////////////////////////////////
////////////////////////////////////////////////////////////////

Defaulter::Defaulter(DefaulterConfig &config) : config_(config) {
  // Set up default LightStateChange object
  if (config.ct_ > 0) {
    defaultLightState_.setColorTemp(config.ct_);
  } else {
    defaultLightState_.setHue(config.color_.hue);
    defaultLightState_.setSaturation(config.color_.saturation);
    defaultLightState_.setBrightness(config.color_.value);
  }
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
      for (auto & it : (**response)) {
        if (needs_update(it.second)) {
          // Perform light state change on this Light
          api.setLightState(it.first, defaultLightState_);
          // TODO: Should we check the result at all?
        }
      }
    }
    nextPollTime_ = now + config_.refresh_rate_;
  }
}

bool Defaulter::needs_update(sphue::Light &light) {
  // TODO: Move "is_default" criteria into DefaulterConfig, give more control over what determines default status
  // TODO: Should we do more checks on the bulb itself?
  //  - is bulb ON
  //  - is colormode "ct" (need to add `colormode` value parsing to sphue::Light)
  // TODO: Keep a local list of Light objects for deeper monitoring of Light property changes
  return light.state().ct() == LIGHT_CT_DEFAULT_VALUE;
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