#include "ppu.h"

namespace game_boy
{
void Ppu::init() {}

void Ppu::tick() {}

void Ppu::oam_write(std::uint16_t addr, std::uint8_t value)
{
    if (addr >= 0xFE00)
    {
        addr -= 0xFE00;
    }

    auto* ptr = (std::uint8_t*)context_.oam_ram.data();
    ptr[addr] = value;
}

std::uint8_t Ppu::oam_read(std::uint16_t addr)
{
    if (addr >= 0xFE00)
    {
        addr -= 0xFE00;
    }

    auto* ptr = (std::uint8_t*)context_.oam_ram.data();
    return ptr[addr];
}

void Ppu::vram_write(std::uint16_t addr, std::uint8_t value)
{
    context_.vram[addr - 0x8000] = value;
}

std::uint8_t Ppu::vram_read(std::uint16_t addr) { return context_.vram[addr - 0x8000]; }

} // namespace game_boy