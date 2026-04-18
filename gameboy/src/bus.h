#pragma once

#include "cart.h"
#include "ram.h"

#include <cstdint>

namespace game_boy
{
class Cpu;

class Bus
{
  public:
    void          insert_cart(Cart* cart);
    void          insert_ram(Ram* ram);
    void          insert_cpu(Cpu* cpu);
    std::uint8_t  read(std::uint16_t addr);
    std::uint16_t read16(std::uint16_t addr);
    void          write(std::uint16_t addr, std::uint8_t val);
    void          write16(std::uint16_t addr, std::uint16_t val);

  private:
    Cart* cart_;
    Cpu*  cpu_;
    Ram*  ram_;
};
} // namespace game_boy