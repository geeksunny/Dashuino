#include "Color.h"
#include <cmath>

namespace color {

inline double &max(double &a, double &b) {
  return (a >= b) ? a : b;
}

inline double &min(double &a, double &b) {
  return (a <= b) ? a : b;
}

////////////////////////////////////////////////////////////////
// Color Factory ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////

template<>
Rgb convert_color<Rgb24, Rgb>(const Rgb24 &color_from) {
  return {(color_from.red / 255.0),
          (color_from.green / 255.0),
          (color_from.blue / 255.0)};
}

template<>
Rgb convert_color<Rgb48, Rgb>(const Rgb48 &color_from) {
  return {(color_from.red / 65535.0),
          (color_from.green / 65535.0),
          (color_from.blue / 65535.0)};
}

template<>
Rgb24 convert_color<Hsv, Rgb24>(const Hsv &color_from) {
  // TODO: implement based on https://www.tlbx.app/color-converter
  return Rgb24();
}

template<>
Rgb24 convert_color<Hsv32, Rgb24>(const Hsv32 &color_from) {
  return convert_color<Hsv, Rgb24>(convert_color<Hsv32, Hsv>(color_from));
}

template<>
Rgb24 convert_color<Rgb48, Rgb24>(const Rgb48 &color_from) {
  return {(uint8_t) std::round((color_from.red / 65535.0) * 255.0),
          (uint8_t) std::round((color_from.green / 65535.0) * 255.0),
          (uint8_t) std::round((color_from.blue / 65535.0) * 255.0)};
}

template<>
Rgb48 convert_color<Rgb24, Rgb48>(const Rgb24 &color_from) {
  return {(uint16_t) std::round((color_from.red / 255.0) * 65535.0),
          (uint16_t) std::round((color_from.green / 255.0) * 65535.0),
          (uint16_t) std::round((color_from.blue / 255.0) * 65535.0)};
}

template<>
Hsv convert_color<Rgb24, Hsv>(const Rgb24 &color_from) {
  Rgb rgb = convert_color<Rgb24, Rgb>(color_from);

  double &cmax = max(rgb.red, max(rgb.green, rgb.blue));
  double &cmin = min(rgb.red, min(rgb.green, rgb.blue));
  double delta = cmax - cmin;

  // hue (in °)
  double h_degrees = (delta == 0.0)
                     ? 0.0
                     : (&cmax == &rgb.red)
                       ? (60.0 * std::fmod(((rgb.green - rgb.blue) / delta), 6))
                       : (&cmax == &rgb.green)
                         ? (60.0 * ((rgb.blue - rgb.red) / delta + 2))
                         : /* (&cmax == &rgb.blue) ? */ (60.0 * ((rgb.red - rgb.green) / delta + 4));
  // saturation
  double s = (delta == 0.0) ? 0.0 : (cmax - cmin) / cmax;
  // value / brightness
  // double v = cmax;

  return {h_degrees, s, cmax};
}

template<>
Hsv convert_color<Hsv32, Hsv>(const Hsv32 &color_from) {
  return {((color_from.hue / 65535.0) * 360.0),
          (color_from.saturation / 100.0),
          (color_from.value / 100.0)};
}

template<>
Hsv32 convert_color<Rgb24, Hsv32>(const Rgb24 &color_from) {
  return convert_color<Hsv, Hsv32>(convert_color<Rgb24, Hsv>(color_from));
}

template<>
Hsv32 convert_color<Hsv, Hsv32>(const Hsv &color_from) {
  return {(uint8_t) std::round(65535 * (color_from.hue / 360.0)),
          (uint8_t) std::round(color_from.saturation * 100),
          (uint8_t) std::round(color_from.value * 100)};
}




}


}

}
