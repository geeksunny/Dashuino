#include "Controller.h"

namespace lightswitch {

////////////////////////////////////////////////////////////////
// Class : Controller //////////////////////////////////////////
////////////////////////////////////////////////////////////////

Controller::Controller()
    : server_(*this),
      sphue_(create_sphue()),
      cycler_(std::bind(&Controller::exec_color, this, std::placeholders::_1, std::placeholders::_2)) {
  //
}

void Controller::loop() {
  server_.loop();
  cycler_.task();
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