#include "io.h"

#include "cpu.h"
#include "timer.h"

#include <iostream>

namespace game_boy
{
std::uint8_t IO::read(std::uint16_t addr)
{
    if (addr == 0xFF01)
    {
        return serial_data_[0];
    }

    if (addr == 0xFF02)
    {
        return serial_data_[1];
    }

    if (BETWEEN(addr, 0xFF04, 0xFF07))
    {
        return Timer::get_instance().read(addr);
    }

    if (addr == 0xFF0F)
    {
        return cpu_->int_flags();
    }

    std::cout << "BUS READ NOT IMPLEMENTED\n";
    return 0;
}

void IO::connect_to_cpu(Cpu* cpu) { cpu_ = cpu; }

IO& IO::get_instance()
{
    static IO io;
    return io;
}

void IO::write(std::uint16_t addr, std::uint8_t val)
{
    if (addr == 0xFF01)
    {
        serial_data_[0] = val;
        return;
    }

    if (addr == 0xFF02)
    {
        serial_data_[1] = val;
        return;
    }

    if (BETWEEN(addr, 0xFF04, 0xFF07))
    {
        Timer::get_instance().write(addr, val);
        return;
    }

    if (addr == 0xFF0F)
    {
        cpu_->set_int_flags(val);
        return;
    }

    std::cout << "BUS WRITE NOT IMPLEMENTED\n";
}

} // namespace game_boy