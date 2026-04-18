#include "ram.h"

#include <format>
#include <stdexcept>

namespace game_boy
{
std::uint8_t Ram::wram_read(std::uint16_t addr)
{
    addr -= 0xC000;

    if (addr >= 0x2000)
    {
        throw std::runtime_error(
            std::format("Invalid Address passed: {:#08X} for wram_read", addr + 0xC000));
    }

    return context_.wram[addr];
}

void Ram::wram_write(std::uint16_t addr, std::uint8_t val)
{
    addr -= 0xC000;

    if (addr >= 0x2000)
    {
        throw std::runtime_error(
            std::format("Invalid Address passed: {:#08X} for wram_write", addr + 0xC000));
    }

    context_.wram[addr] = val;
}

std::uint8_t Ram::hram_read(std::uint16_t addr)
{
    addr -= 0xFF80;

    if (addr >= 0x80)
    {
        throw std::runtime_error(
            std::format("Invalid Address passed: {:#08X}  for hram_read", addr + 0xFF80));
    }

    return context_.hram[addr];
}

void Ram::hram_write(std::uint16_t addr, std::uint8_t val)
{
    addr -= 0xFF80;

    if (addr >= 0x80)
    {
        throw std::runtime_error(
            std::format("Invalid Address passed: {:#08X} for hram_write", addr + 0xFF80));
    }

    context_.hram[addr] = val;
}

} // namespace game_boy