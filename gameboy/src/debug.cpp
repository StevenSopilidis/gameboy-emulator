#include "debug.h"

#include "bus.h"

#include <format>
#include <iostream>

namespace game_boy
{
void Debug::insert_bus(Bus* bus) { bus_ = bus; }

void Debug::update()
{
    if (bus_->read(0xFF02) == 0x81)
    {
        auto data         = bus_->read(0xFF01);
        msg_[msg_size_++] = data;

        bus_->write(0xFF02, 0);
    }
}

void Debug::print()
{
    if (msg_[0] != 0)
    {
        std::cout << std::format("DBG: {}\n", msg_.data());
    }
}

} // namespace game_boy