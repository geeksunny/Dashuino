#ifndef LIGHTSWITCH_INCLUDE_COLOR_H_
#define LIGHTSWITCH_INCLUDE_COLOR_H_

#include <cstdint>

namespace color {

struct Rgb {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

struct Hsl {
  double hue;
  double saturation;
  double lightness;
};

struct HslInt {
  uint16_t hue;
  uint8_t saturation;
  uint8_t lightness;
};

struct Hsv {
  double hue;
  double saturation;
  double value;
};

struct HsvInt {
  uint16_t hue;
  uint8_t saturation;
  uint8_t value;
};

Rgb hsl_to_rgb(const Hsl &hsl);
Hsl rgb_to_hsl(const Rgb &rgb);

HslInt hsl_to_hsl_int(const Hsl &hsl);
Hsl hsl_int_to_hsl(const HslInt &hsl_int);

Rgb hsv_to_rgb(const Hsv &hsv);
Hsv rgb_to_hsv(const Rgb &rgb);

HsvInt hsv_to_hsv_int(const Hsv &hsv);
Hsv hsv_int_to_hsv(const HsvInt &hsv_int);

class Color : public Rgb {
 public:
  explicit Color(uint8_t red, uint8_t green, uint8_t blue) : Rgb{red, green, blue} {}

  Color &fromRgb(const Rgb &rgb);
  Color &fromHsl(const Hsl &hsl);
  Color &fromHslInt(const HslInt &hsl_int);
  Color &fromHsv(const Hsv &hsv);
  Color &fromHsvInt(const HsvInt &hsv_int);

  Hsl toHsl();
  HslInt toHslInt();

  Hsv toHsv();
  HsvInt toHsvInt();
};

}

#endif //LIGHTSWITCH_INCLUDE_COLOR_H_
