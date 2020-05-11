#ifndef LIGHTSWITCH_INCLUDE_CONTROLLER_H_
#define LIGHTSWITCH_INCLUDE_CONTROLLER_H_

#include <LightswitchServer.h>
#include <Sphue.h>
#include "Configuration.h"

namespace lightswitch {

class Defaulter {
  friend class Controller;

 public:
  explicit Defaulter(DefaulterConfig &config);

  explicit operator bool();
  explicit operator bool() const;

 private:
  DefaulterConfig &config_;
  sphue::LightStateChange defaultLightState_;
  unsigned long nextPollTime_ = 0;

  void loop(sphue::Sphue &api);
  bool needs_update(sphue::Light &light);
};

class Controller : public ActionHandler {
 public:
  explicit Controller(Configuration &configuration);
  void loop();
  void exec_color(uint8_t cycle_id, color::Hsv32 *color);
 private:
  LightswitchServer server_;
  Configuration &config_;
  sphue::Sphue sphue_;
  Defaulter defaulter_;
  color::ColorCycler<color::Hsv32> cycler_;

  sphue::Sphue create_sphue();
  bool onAction(uint8_t action, uint8_t value) override;
};

}

#endif //LIGHTSWITCH_INCLUDE_CONTROLLER_H_
