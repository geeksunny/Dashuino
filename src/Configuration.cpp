#include "Configuration.h"
#include <PgmStringTools.hpp>
#include <EnumTools.hpp>
#include <SD.h>

#define PORT_HTTP   80
#define PORT_HTTPS  443

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
const char key_ct[] PROGMEM = "ct";
const char key_refresh_rate[] PROGMEM = "refresh_rate";

const char key_colors[] PROGMEM = "colors";
const char key_comment[] PROGMEM = "comment";
const char key_cycle[] PROGMEM = "cycle";
const char key_duration[] PROGMEM = "duration";
const char key_fade_duration[] PROGMEM = "fade_duration";
const char key_group[] PROGMEM = "group";
const char key_light[] PROGMEM = "light";
const char key_steps[] PROGMEM = "steps";

const char key_hostname[] PROGMEM = "hostname";
const char key_port[] PROGMEM = "port";
const char key_auto[] PROGMEM = "auto";
const char key_protocol[] PROGMEM = "protocol";
const char key_require_self_signed[] PROGMEM = "require_self_signed";
const char key_key[] PROGMEM = "key";

// Values
const char value_rgb[] PROGMEM = "rgb";
const char value_rgb24[] PROGMEM = "rgb24";
const char value_rgb48[] PROGMEM = "rgb48";
const char value_hsv[] PROGMEM = "hsv";
const char value_hsv32[] PROGMEM = "hsv32";

const char value_http[] PROGMEM = "http";
const char value_https[] PROGMEM = "https";
}

////////////////////////////////////////////////////////////////
// Class : ConfigColor /////////////////////////////////////////
////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////
// Class : DefaulterConfig /////////////////////////////////////
////////////////////////////////////////////////////////////////

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
  STR_EQ_RET(strings::key_ct, parser.get(ct_))
  STR_EQ_RET(strings::key_refresh_rate, parser.get(refresh_rate_))
  return false;
}

////////////////////////////////////////////////////////////////
// Class : SphueConfig /////////////////////////////////////////
////////////////////////////////////////////////////////////////

const String &SphueConfig::hostname() const {
  return hostname_;
}

uint16_t SphueConfig::port() const {
  return port_ ? port_ : ((protocol_ == HTTPS) ? PORT_HTTPS : PORT_HTTP);
}

SphueConfig::Protocol SphueConfig::protocol() const {
  return protocol_;
}

bool SphueConfig::autoDiscover() const {
  return auto_discover_ || hostname_.isEmpty();
}

bool SphueConfig::requireSelfSigned() const {
  return require_self_signed_;
}

const String &SphueConfig::apiKey() const {
  return api_key_;
}

MAKE_ENUM_MAP(protocol_map, SphueConfig::Protocol,
              MAPPING(SphueConfig::Protocol::HTTP, strings::value_http),
              MAPPING(SphueConfig::Protocol::HTTPS, strings::value_https)
)

bool SphueConfig::onKey(String &key, json::JsonParser &parser) {
  STR_EQ_INIT(key.c_str())
  STR_EQ_RET(strings::key_hostname, parser.get(hostname_))
  STR_EQ_RET(strings::key_port, parser.get(port_))
  STR_EQ_DO(strings::key_protocol, {
    String protocol;
    bool success = parser.get(protocol);
    protocol_ = pgm_string_to_enum(protocol.c_str(), Protocol::HTTP, protocol_map);
    return success;
  })
  STR_EQ_RET(strings::key_auto, parser.get(auto_discover_))
  STR_EQ_RET(strings::key_require_self_signed, parser.get(require_self_signed_))
  STR_EQ_RET(strings::key_key, parser.get(api_key_))
  return false;
}

////////////////////////////////////////////////////////////////
// Class : Configuration ///////////////////////////////////////
////////////////////////////////////////////////////////////////

bool Configuration::load() {
  String filename;
  File file;
  bool success_default, success_sphue, success_switches;

  if (!SD.begin(PIN_SD_CS)) {
    return false;
  }

  // Defaulter
  {
    filename = read_prog_str(strings::filename_defaulter);
    file = SD.open(filename, FILE_READ);
    json::JsonParser parser(file);
    success_default = parser.get(defaulter_config_);
    file.close();
  }

  // Sphue
  {
    filename = read_prog_str(strings::filename_sphue);
    file = SD.open(filename, FILE_READ);
    json::JsonParser parser(file);
    success_sphue = parser.get(sphue_config_);
    file.close();
  }

  // Switches
  {
    filename = read_prog_str(strings::filename_switches);
    file = SD.open(filename, FILE_READ);
    json::JsonParser parser(file);
    success_switches = parser.get(switches_config_);
    file.close();
  }

  return success_default && success_sphue && success_switches;
}

}
