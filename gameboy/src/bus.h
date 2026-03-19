#pragma once

#include <cstdint>
namespace game_boy
{
class Bus
{
  public:
    std::uint8_t read(std::uint16_t addr);
    void         write(std::uint16_t addr, std::uint8_t val);
};
} // namespace game_boy