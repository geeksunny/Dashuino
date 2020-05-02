#include "Configuration.h"
#include <PgmStringTools.hpp>
#include <EnumTools.hpp>
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
const char key_value[] PROGMEM = "value";
const char key_refresh_rate[] PROGMEM = "refresh_rate";

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

const char value_http[] PROGMEM = "http";
const char value_https[] PROGMEM = "https";
}

MAKE_ENUM_MAP(color_format_map, ColorFormat,
              MAPPING(ColorFormat::Rgb, strings::value_rgb),
              MAPPING(ColorFormat::Rgb24, strings::value_rgb24),
              MAPPING(ColorFormat::Rgb48, strings::value_rgb48),
              MAPPING(ColorFormat::Hsv, strings::value_hsv),
              MAPPING(ColorFormat::Hsv32, strings::value_hsv32)
)

color::Hsv32 ConfigColor::getColor() {
  // Convert ConfigColor data into color::Hsv32
  switch (format_) {
    case ColorFormat::Rgb: {
      color::Rgb value = {value_[0], value_[1], value_[2]};
      return color::convert_color<color::Rgb, color::Hsv32>(value);
    }
    case ColorFormat::Rgb24: {
      color::Rgb24 value = {(uint8_t) value_[0], (uint8_t) value_[1], (uint8_t) value_[2]};
      return color::convert_color<color::Rgb24, color::Hsv32>(value);
    }
    case ColorFormat::Rgb48: {
      color::Rgb48 value = {(uint16_t) value_[0], (uint16_t) value_[1], (uint16_t) value_[2]};
      return color::convert_color<color::Rgb48, color::Hsv32>(value);
    }
    case ColorFormat::Hsv: {
      color::Hsv value = {value_[0], value_[1], value_[2]};
      return color::convert_color<color::Hsv, color::Hsv32>(value);
    }
    case ColorFormat::Hsv32: {
      return {(uint16_t) value_[0], (uint8_t) value_[1], (uint8_t) value_[2]};
    }
    case ColorFormat::UNKNOWN:
    default:
      return {};
  }
}

bool ConfigColor::onKey(String &key, json::JsonParser &parser) {
  STR_EQ_INIT(key.c_str())
  STR_EQ_DO(strings::key_format, {
    String format;
    bool success = parser.get(format);
    format_ = pgm_string_to_enum(format.c_str(), ColorFormat::UNKNOWN, color_format_map);
    return success;
  })
  STR_EQ_DO(strings::key_value, {
    if (parser.checkValueType() != json::ARRAY) {
      parser.skipValue();
      return false;
    }
    json::JsonArrayIterator<double> array = parser.iterateArray<double>();
    bool success = false;
    for (auto &value : value_) {
      success = array.hasNext() && array.getNext(value);
      if (!success) {
        break;
      }
    }
    array.finish();
    return success;
  })
  return false;
}

bool DefaulterConfig::onKey(String &key, json::JsonParser &parser) {
  STR_EQ_INIT(key.c_str())
  STR_EQ_RET(strings::key_enabled, parser.get(enabled_))
  STR_EQ_DO(strings::key_color, {
    ConfigColor color;
    if (parser.get(color)) {
      color_ = color.getColor();
      return true;
    }
    return false;
  })
  STR_EQ_RET(strings::key_refresh_rate, parser.get(refresh_rate_))
  return false;
}

MAKE_ENUM_MAP(protocol_map, SphueConfig::Protocol,
              MAPPING(SphueConfig::Protocol::HTTP, strings::value_http),
              MAPPING(SphueConfig::Protocol::HTTPS, strings::value_https)
)

bool SphueConfig::onKey(String &key, json::JsonParser &parser) {
  STR_EQ_INIT(key.c_str())
  STR_EQ_DO(strings::key_protocol, {
    String protocol;
    bool success = parser.get(protocol);
    protocol_ = pgm_string_to_enum(protocol.c_str(), Protocol::HTTP, protocol_map);
    return success;
  })
  STR_EQ_RET(strings::key_require_self_signed, parser.get(require_self_signed_))
  return false;
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
