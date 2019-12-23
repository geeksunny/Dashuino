#ifndef LIGHTSWITCH_INCLUDE_COLORCYCLER_H_
#define LIGHTSWITCH_INCLUDE_COLORCYCLER_H_

#include "Color.h"
#include <memory>
#include <deque>

#define SAFE_ARR_SIZE(begin, end) ((end - begin) > 0) ? (end - begin) : 0

namespace color {

template<typename ColorType>
class ColorCycle {
 public:
  explicit ColorCycle(const long color_duration, // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
                      std::initializer_list<ColorType> colors,
                      const uint8_t fade_step_count = 0,
                      const long fade_duration = 0)
      : ColorCycle(color_duration, colors.begin(), colors.end(), fade_step_count, fade_duration) {}
  explicit ColorCycle(long color_duration,
                      const ColorType *colors_begin,
                      const ColorType *colors_end,
                      uint8_t fade_step_count = 0,
                      long fade_duration = 0);

  ColorType *get(unsigned long now);

 private:
  std::unique_ptr<std::unique_ptr<ColorType[]>[]> cycles_;
  uint8_t color_count_;
  uint8_t fade_step_count_;
  long color_duration_;
  long fade_step_duration_;

  uint8_t color_index_ = 0;
  uint8_t step_index_ = 0;
  unsigned long next_action_ = 0;
};

template<typename InputColorType, typename OutputColorType>
ColorCycle<OutputColorType> create_cycle(const long color_duration,
                                         const InputColorType *colors_begin,
                                         const InputColorType *colors_end,
                                         uint8_t fade_step_count = 0,
                                         long fade_duration = 0) {
  uint8_t size = SAFE_ARR_SIZE(colors_begin, colors_end);
  OutputColorType colors[size];
  for (uint8_t i = 0; i < size; ++i) {
    colors[i] = convert_color<InputColorType, OutputColorType>(*(colors_begin + i));
  }
  return ColorCycle<OutputColorType>(color_duration, colors, colors + size, fade_step_count, fade_duration);
}

template<typename ColorType>
class ColorCycler {
 public:
  explicit ColorCycler(std::function<void(int, ColorType *)> callback);
  void insert(uint8_t cycle_id, ColorCycle<ColorType> &color_cycle);
  void task();
 private:
  std::function<void(int, ColorType *)> callback_;
  std::deque<std::pair<uint8_t, ColorCycle<ColorType>>> cycle_index_;
};

}

#endif //LIGHTSWITCH_INCLUDE_COLORCYCLER_H_
