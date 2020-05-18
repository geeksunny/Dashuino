#ifndef LIGHTSWITCH_INCLUDE_CONFIGURATION_H_
#define LIGHTSWITCH_INCLUDE_CONFIGURATION_H_

#include <JSON.h>
#include "ColorCycler.h"

#define COLOR_VALUE_SIZE 3

namespace lightswitch {

enum class ColorFormat {
  UNKNOWN, Rgb, Rgb24, Rgb48, Hsv, Hsv32
};

class ConfigColor : public json::JsonModel {
 public:
  color::Hsv32 getColor();

 private:
  ColorFormat format_;
  double value_[COLOR_VALUE_SIZE];

  bool onKey(String &key, json::JsonParser &parser) override;
};

class DefaulterConfig : public json::JsonModel {
  friend class Controller;
  friend class Defaulter;

 private:
  bool enabled_;
  color::Hsv32 color_;
  uint16_t ct_;
  unsigned long refresh_rate_;

  bool onKey(String &key, json::JsonParser &parser) override;
};

class SphueConfig : public json::JsonModel {
 public:
  enum Protocol {
    HTTP, HTTPS
  };

  const String &hostname() const;
  uint16_t port() const;
  Protocol protocol() const;
  bool autoDiscover() const;
  bool requireSelfSigned() const;
  const String &apiKey() const;

 private:
  String hostname_;
  uint16_t port_;
  Protocol protocol_;
  bool auto_discover_;
  bool require_self_signed_;
  String api_key_;

  bool onKey(String &key, json::JsonParser &parser) override;
};

class CycleConfig : public json::JsonModel {
  friend class Controller;
 private:
  ColorFormat format_;
//  std::deque<> colors_;
  unsigned long duration_;
  uint8_t steps_;
  unsigned long fade_duration_;

  bool onKey(String &key, json::JsonParser &parser) override;
};

class Switch : public json::JsonModel {
 public:
  enum Type {
    Light, Group
  };
 private:
  // TODO: Should we parse `comment` into memory?
  Type type_;
  uint8_t id_;
  CycleConfig cycle_config_;
//  std::unique_ptr<color::ColorCycle<color::Hsv32>> cycle_;

  bool onKey(String &key, json::JsonParser &parser) override;
};

class SwitchesConfig : public json::JsonModel {
  friend class Controller;
 private:
  std::map<uint8_t, Switch> switches_;

  bool onKey(String &key, json::JsonParser &parser) override;
};

class Configuration {
  friend class Controller;
 public:
  Configuration() = default;
  bool load();
//  bool save();
 private:
  DefaulterConfig defaulter_config_;
  SphueConfig sphue_config_;
  SwitchesConfig switches_config_;
};

}

#endif //LIGHTSWITCH_INCLUDE_CONFIGURATION_H_
