#include "ColorCycler.h"
#include <Arduino.h>

namespace color {

////////////////////////////////////////////////////////////////
// Utility Functions/ //////////////////////////////////////////
////////////////////////////////////////////////////////////////

template<typename T>
std::unique_ptr<T[]> make_unique_array(int arr_size) {
  return std::unique_ptr<T[]>(new T[arr_size]);
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
    result_arr[i] = convert_color<Rgb, ColorType>(faded_arr[i]);
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
        color_a.red - (step_r * i),
        color_a.green - (step_g * i),
        color_a.blue - (step_b * i),
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
      steps.get()[0].get()[0] = *colors_begin;
    } else {
      const ColorType *last_color = colors_end - 1;
      const ColorType *color_a, *color_b;
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
ColorCycle<ColorType>::ColorCycle(const long color_duration,
                                  const ColorType *colors_begin,
                                  const ColorType *colors_end,
                                  const uint8_t fade_step_count,
                                  const long fade_duration)
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

template<typename ColorType>
ColorType *ColorCycle<ColorType>::get(const unsigned long now) {
  if (now > next_action_) {
    // TODO: Process color loop
    //  - update next_action_ value
    //  - return pointer to next color cycle value
  }
  return nullptr;
}

template
class ColorCycle<Rgb>;
template
class ColorCycle<Rgb24>;
template
class ColorCycle<Rgb48>;
template
class ColorCycle<Hsv>;
template
class ColorCycle<Hsv32>;

////////////////////////////////////////////////////////////////
// Class : ColorCycler /////////////////////////////////////////
////////////////////////////////////////////////////////////////

template<typename ColorType>
ColorCycler<ColorType>::ColorCycler(std::function<void(int, ColorType *)> callback) : callback_(callback) {
  //
}

template<typename ColorType>
void ColorCycler<ColorType>::insert(const uint8_t cycle_id, ColorCycle<ColorType> &color_cycle) {
  cycle_index_.push_back({cycle_id, std::move(color_cycle)});
}

template<typename ColorType>
void ColorCycler<ColorType>::task() {
  unsigned long now = millis();
  ColorType *color;
  for (auto it = cycle_index_.begin(); it != cycle_index_.end(); ++it) {
    color = (*it).second.get(now);
    if (nullptr != color) {
      callback_((*it).first, color);
    }
  }
}

template
class ColorCycler<Rgb>;
template
class ColorCycler<Rgb24>;
template
class ColorCycler<Rgb48>;
template
class ColorCycler<Hsv>;
template
class ColorCycler<Hsv32>;

}