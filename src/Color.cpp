#include "Color.h"
#include <cmath>

namespace color {

inline double &max(double &a, double &b) {
  return (a >= b) ? a : b;
}

inline double &min(double &a, double &b) {
  return (a <= b) ? a : b;
}

Rgb hsl_to_rgb(const Hsl &hsl) {
  // TODO
  return Rgb();
}

Hsl rgb_to_hsl(const Rgb &rgb) {
  double r = rgb.red / 255.0;
  double g = rgb.green / 255.0;
  double b = rgb.blue / 255.0;

  double &cmax = max(r, max(g, b));
  double &cmin = min(r, min(g, b));
  double delta = cmax - cmin;

  // hue (in °)
  double h_degrees = (delta == 0.0)
                     ? 0.0
                     : (&cmax == &r)
                       ? (60.0 * std::fmod(((g - b) / delta), 6))
                       : (&cmax == &g)
                         ? (60.0 * ((b - r) / delta + 2))
                         : /* (&cmax == &b) ? */ (60.0 * ((r - g) / delta + 4));
  // saturation
  double s = (delta == 0.0) ? 0.0 : (delta / (1.0 - std::abs(cmax + cmin - 1)));
  // lightness
  double l = (cmax + cmin) / 2.0;

  return {h_degrees, s, l};
}

inline HslInt hsl_to_hsl_int(const Hsl &hsl) {
  return {(uint8_t) std::round(65535 * (hsl.hue / 360.0)),
          (uint8_t) std::round(hsl.saturation * 100),
          (uint8_t) std::round(hsl.lightness * 100)};
}

inline Hsl hsl_int_to_hsl(const HslInt &hsl_int) {
  return {((hsl_int.hue / 65535.0) * 360.0),
          (hsl_int.saturation / 100.0),
          (hsl_int.lightness / 100.0)};
}

Rgb hsv_to_rgb(const Hsv &hsv) {
  // TODO
  return Rgb();
}

Hsv rgb_to_hsv(const Rgb &rgb) {
  double r = rgb.red / 255.0;
  double g = rgb.green / 255.0;
  double b = rgb.blue / 255.0;

  double &cmax = max(r, max(g, b));
  double &cmin = min(r, min(g, b));
  double delta = cmax - cmin;

  // hue (in °)
  double h_degrees = (delta == 0.0)
                     ? 0.0
                     : (&cmax == &r)
                       ? (60.0 * std::fmod(((g - b) / delta), 6))
                       : (&cmax == &g)
                         ? (60.0 * ((b - r) / delta + 2))
                         : /* (&cmax == &b) ? */ (60.0 * ((r - g) / delta + 4));
  // saturation
  double s = (delta == 0.0) ? 0.0 : (cmax - cmin) / cmax;
  // value / brightness
  // double v = cmax;

  return {h_degrees, s, cmax};
}

inline HsvInt hsv_to_hsv_int(const Hsv &hsv) {
  return {(uint8_t) std::round(65535 * (hsv.hue / 360.0)),
          (uint8_t) std::round(hsv.saturation * 100),
          (uint8_t) std::round(hsv.value * 100)};
}

inline Hsv hsv_int_to_hsv(const HsvInt &hsv_int) {
  return {((hsv_int.hue / 65535.0) * 360.0),
          (hsv_int.saturation / 100.0),
          (hsv_int.value / 100.0)};
}

Color &Color::fromRgb(const Rgb &rgb) {
  red = rgb.red;
  green = rgb.green;
  blue = rgb.blue;
  return *this;
}

Color &Color::fromHsl(const Hsl &hsl) {
  return fromRgb(hsl_to_rgb(hsl));
}

Color &Color::fromHslInt(const HslInt &hsl_int) {
  return fromRgb(hsl_to_rgb(hsl_int_to_hsl(hsl_int)));
}

Color &Color::fromHsv(const Hsv &hsv) {
  return fromRgb(hsv_to_rgb(hsv));
}

Color &Color::fromHsvInt(const HsvInt &hsv_int) {
  return fromRgb(hsv_to_rgb(hsv_int_to_hsv(hsv_int)));
}

Hsl Color::toHsl() {
  return rgb_to_hsl(*this);
}

HslInt Color::toHslInt() {
  return hsl_to_hsl_int(rgb_to_hsl(*this));
}

Hsv Color::toHsv() {
  return rgb_to_hsv(*this);
}

HsvInt Color::toHsvInt() {
  return hsv_to_hsv_int(rgb_to_hsv(*this));
}

}
