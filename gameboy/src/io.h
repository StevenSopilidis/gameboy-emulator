#pragma once

#include "common.h"

#include <array>

namespace game_boy
{
class Timer;

class IO
{
  public:
    std::uint8_t read(std::uint16_t addr);
    void         write(std::uint16_t addr, std::uint8_t val);

    static IO& get_instance();

  private:
    std::array<uint8_t, 2> serial_data_;
};
} // namespace game_boy