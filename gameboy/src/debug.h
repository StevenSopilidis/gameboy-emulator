#pragma once

#include "common.h"

#include <array>

namespace game_boy
{
class Bus;

class Debug
{
  public:
    void update();
    void print();
    void insert_bus(Bus* bus);

  private:
    std::array<char, 1024> msg_{};
    std::size_t            msg_size_{0};
    Bus*                   bus_{nullptr};
};
} // namespace game_boy