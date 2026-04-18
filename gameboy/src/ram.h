#pragma once

#include <array>
#include <cstdint>
namespace game_boy
{

struct RamContext
{
    std::array<std::uint8_t, 0x2000> wram;
    std::array<std::uint8_t, 0x80>   hram;
};

class Ram
{
  public:
    std::uint8_t wram_read(std::uint16_t addr);
    void         wram_write(std::uint16_t addr, std::uint8_t val);

    std::uint8_t hram_read(std::uint16_t addr);
    void         hram_write(std::uint16_t addr, std::uint8_t val);

  private:
    RamContext context_;
};
} // namespace game_boy