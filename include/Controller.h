#ifndef LIGHTSWITCH_INCLUDE_CONTROLLER_H_
#define LIGHTSWITCH_INCLUDE_CONTROLLER_H_

#include <LightswitchServer.h>
#include <Sphue.h>
#include "ColorCycler.h"

namespace lightswitch {

class Controller : public ActionHandler {
 public:
  explicit Controller();
  void loop();
  void exec_color(uint8_t cycle_id, color::Hsv32 *color);
 private:
  LightswitchServer server_;
  sphue::Sphue sphue_;
  color::ColorCycler<color::Hsv32> cycler_;

  sphue::Sphue create_sphue();
  bool onAction(uint8_t action, uint8_t value) override;
};

}

#endif //LIGHTSWITCH_INCLUDE_CONTROLLER_H_
