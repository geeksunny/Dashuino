#include "Color.h"
#include <cmath>

#define SAFE_ARR_SIZE(begin, end) std::max(end - begin, 0)

namespace color {

////////////////////////////////////////////////////////////////
// Utility Functions/ //////////////////////////////////////////
////////////////////////////////////////////////////////////////

template<typename T>
std::unique_ptr<T[]> make_unique_array(int arr_size) {
  return std::unique_ptr<T[]>(new T[arr_size]);
}

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

////////////////////////////////////////////////////////////////
// Color Step Generation////////////////////////////////////////
////////////////////////////////////////////////////////////////

template<typename ColorType>
std::unique_ptr<ColorType[]> fade_colors(const uint8_t step_count, const ColorType &color_a, const ColorType &color_b) {
  // TODO: Find a better way to approach this which doesn't involve so many temporary unique_ptr<Rgb[]>
  //  to be allocated when processing non-Rgb ColorCycles...
  Rgb rgb_a = convert_color<ColorType, Rgb>(color_a);
  Rgb rgb_b = convert_color<ColorType, Rgb>(color_b);
  auto faded = fade_colors(step_count, rgb_a, rgb_b);
  Rgb *faded_arr = faded.get();
  auto result = make_unique_array<ColorType>(step_count);
  ColorType *result_arr = result.get();
  for (uint8_t i = 0; i < step_count; ++i) {
    result_arr[i] = convert_color<Rgb, ColorType>(&faded_arr[i]);
  }
  return result;
}

template<>
std::unique_ptr<Rgb[]> fade_colors(const uint8_t step_count, const Rgb &color_a, const Rgb &color_b) {
  double step_r = (color_a.red - color_b.red) / step_count;
  double step_g = (color_a.green - color_b.green) / step_count;
  double step_b = (color_a.blue - color_b.blue) / step_count;
  auto result = make_unique_array<Rgb>(step_count);
  Rgb *result_arr = result.get();
  for (uint8_t i = 0; i < step_count; ++i) {
    result_arr[i] = {
        color_a.red + (step_r * i),
        color_a.green + (step_g * i),
        color_a.blue + (step_b * i),
    };
  }
  return result;
}

template<typename ColorType>
std::unique_ptr<std::unique_ptr<ColorType[]>[]> make_cycle_steps(const uint8_t step_count,
                                                                 const ColorType *colors_begin,
                                                                 const ColorType *colors_end) {
  uint8_t size = SAFE_ARR_SIZE(colors_begin, colors_end);
  auto steps = make_unique_array<std::unique_ptr<ColorType[]>>(size);
  if (size > 0) {
    if (size == 1 || step_count <= 1) {
      steps.get()[0] = make_unique_array<ColorType>(1);
      steps.get()[0].get()[0] = &colors_begin;
    } else {
      ColorType *last_color = colors_end - 1;
      ColorType *color_a, *color_b;
      int i;
      for (i = 0; i < (size - 1); ++i) {
        color_a = colors_begin + i;
        color_b = color_a + 1;
        steps.get()[i] = fade_colors(step_count, *color_a, *color_b);
      }
      steps.get()[i] = fade_colors(step_count, *last_color, *colors_begin);
    }
  }
  return steps;
}

////////////////////////////////////////////////////////////////
// Class : ColorCycle //////////////////////////////////////////
////////////////////////////////////////////////////////////////

template<typename ColorType>
ColorCycle<ColorType>::ColorCycle(long color_duration,
                                  ColorType *colors_begin,
                                  ColorType *colors_end,
                                  uint8_t fade_step_count,
                                  long fade_duration)
    : cycles_(make_cycle_steps(fade_step_count, colors_begin, colors_end)),
      color_count_(SAFE_ARR_SIZE(colors_begin, colors_end)),
      fade_step_count_(fade_step_count),
      color_duration_(color_duration) {
  if (fade_step_count > 1) {
    // TODO: Add math/logic to account for uneven fade_step_duration_ values; account for the remainder
    if (fade_duration == 0) {
      fade_step_duration_ = color_duration / fade_step_count;
    } else {
      fade_step_duration_ = fade_duration / fade_step_count;
    }
  } else {
    fade_step_duration_ = 0;
  }
}

}
