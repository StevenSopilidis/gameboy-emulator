#pragma once

#include "cart.h"

#include <cstdint>

namespace game_boy
{
class Bus
{
  public:
    void         insert_cart(Cart* cart) ;
    std::uint8_t read(std::uint16_t addr);
    void         write(std::uint16_t addr, std::uint8_t val);

  private:
    Cart* cart_;
};
} // namespace game_boy