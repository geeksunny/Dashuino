#include "Configuration.h"
#include <prog_str.h>
#include <SD.h>

namespace lightswitch {

namespace strings {
// Config filenames
const char filename_defaulter[] PROGMEM = "defaulter.json";
const char filename_sphue[] PROGMEM = "sphue.json";
const char filename_switches[] PROGMEM = "switches.json";

// Keys
const char key_color[] PROGMEM = "color";
const char key_enabled[] PROGMEM = "enabled";
const char key_format[] PROGMEM = "format";

const char key_colors[] PROGMEM = "colors";
const char key_comment[] PROGMEM = "comment";
const char key_cycle[] PROGMEM = "cycle";
const char key_duration[] PROGMEM = "duration";
const char key_fade_duration[] PROGMEM = "fade_duration";
const char key_group[] PROGMEM = "group";
const char key_light[] PROGMEM = "light";
const char key_steps[] PROGMEM = "steps";

const char key_protocol[] PROGMEM = "protocol";
const char key_require_self_signed[] PROGMEM = "require_self_signed";

// Values
const char value_rgb[] PROGMEM = "rgb";
const char value_rgb24[] PROGMEM = "rgb24";
const char value_rgb48[] PROGMEM = "rgb48";
const char value_hsv[] PROGMEM = "hsv";
const char value_hsv32[] PROGMEM = "hsv32";
}

bool Configuration::load() {
  String filename;
  File file;
  bool success_default, success_sphue, success_switches;

  // Defaulter
  {
    filename = read_prog_str(strings::filename_defaulter);
    file = SD.open(filename, FILE_READ);
    json::JsonParser parser(file);
    success_default = parser.get(defaulter_config_);
  }

  // Sphue
  {
    filename = read_prog_str(strings::filename_sphue);
    file = SD.open(filename, FILE_READ);
    json::JsonParser parser(file);
    success_sphue = parser.get(sphue_config_);
  }

  // Switches
  {
    filename = read_prog_str(strings::filename_switches);
    file = SD.open(filename, FILE_READ);
    json::JsonParser parser(file);
    success_switches = parser.get(switches_config_);
  }

  return success_default && success_sphue && success_switches;
}

}
