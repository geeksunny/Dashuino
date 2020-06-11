#include "Controller.h"

#define LIGHT_CT_DEFAULT_VALUE 366

#define LED_DURATION_LONG      2500
#define LED_DURATION_SHORT     500

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
  return light.state().on() && light.state().ct() == LIGHT_CT_DEFAULT_VALUE;
}

////////////////////////////////////////////////////////////////
// Class : Controller //////////////////////////////////////////
////////////////////////////////////////////////////////////////

Controller::Controller(Configuration &configuration, led::LED *led_primary, led::LED *led_secondary)
    : server_(*this),
      config_(configuration),
      ledPrimary_(led_primary),
      ledSecondary_(led_secondary),
      sphue_(create_sphue()),
      defaulter_(configuration.defaulter_config_),
      cycler_(std::bind(&Controller::exec_color, this, std::placeholders::_1, std::placeholders::_2)) {
  //
  LightswitchConfig &ls = configuration.lightswitch_config_;
  // ServerInterfaces are allocated on the heap as they are registered.
  // We are not keeping track of their pointers here as they are never intended to be deleted.
  if (ls.tcp_) {
    server_.registerInterface(new TcpInterface());
  }
  if (ls.udp_) {
    UdpInterface udp;
    server_.registerInterface(new UdpInterface());
  }
  if (ls.dhcp_) {
    // TODO: Add DhcpInterface when implemented upstream
  }
  if (ls.espnow_) {
    server_.registerInterface(new EspNowInterface());
  }
}

void Controller::setup() {
  server_.setup();
  // TODO: If server setup fails, display error pattern on LEDs
  // TODO: Check status of Sphue object. Display LED status
}

void Controller::loop() {
  server_.loop();
  cycler_.task();
  if (defaulter_) {
    defaulter_.loop(sphue_);
  }
  // Call LED looper to process timed jobs
  led::LED::loop();
}

void Controller::exec_color(uint8_t cycle_id, color::Hsv32 *color) {
  // TODO: assign color to specified target via cycle_id
}

sphue::Sphue Controller::create_sphue() {
  if (!config_.sphue_config_.autoDiscover()) {
    if (config_.sphue_config_.apiKey().length()) {
      return sphue::Sphue(config_.sphue_config_.apiKey().c_str(),
                          config_.sphue_config_.hostname().c_str(),
                          config_.sphue_config_.port());
    }
    return sphue::Sphue(config_.sphue_config_.hostname().c_str(), config_.sphue_config_.port());
  } else {
    return sphue::autoDiscoverHub();
    // TODO: Should auto-discovered hostnames be saved for future use?
    //  - If yes, check result here, store on success
  }
}

bool Controller::onAction(uint8_t action, uint8_t value) {
  // TODO: handle action, dispatch commands to sphue object
  return false;
}

}