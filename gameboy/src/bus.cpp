#include "bus.h"

#include "cpu.h"

#include <iostream>
#include <stdexcept>

// memory MAP
// 0x0000 - 0x3FFF : ROM Bank 0
// 0x4000 - 0x7FFF : ROM Bank 1 - Switchable
// 0x8000 - 0x97FF : CHR RAM
// 0x9800 - 0x9BFF : BG Map 1
// 0x9C00 - 0x9FFF : BG Map 2
// 0xA000 - 0xBFFF : Cartridge RAM
// 0xC000 - 0xCFFF : RAM Bank 0
// 0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
// 0xE000 - 0xFDFF : Reserved - Echo RAM
// 0xFE00 - 0xFE9F : Object Attribute Memory
// 0xFEA0 - 0xFEFF : Reserved - Unusable
// 0xFF00 - 0xFF7F : I/O Registers
// 0xFF80 - 0xFFFE : Zero Page (High ram)

namespace game_boy
{
void Bus::insert_cart(Cart* cart) { cart_ = cart; }

void Bus::insert_ram(Ram* ram) { ram_ = ram; }

void Bus::insert_cpu(Cpu* cpu) { cpu_ = cpu; }

std::uint8_t Bus::read(std::uint16_t addr)
{
    if (addr < 0x8000)
    {
        // ROM data
        return cart_->read(addr);
    }

    if (addr < 0xA000)
    {
        throw std::runtime_error("BUS READ NOT IMPLEMENTED");
    }

    if (addr < 0xC000)
    {
        // Cartridge ram
        cart_->read(addr);
    }

    if (addr < 0xE000)
    {
        // WRAM (Working RAM)
        return ram_->wram_read(addr);
    }

    if (addr < 0xFE00)
    {
        // reserved echo ram
        return 0;
    }

    if (addr < 0xFEA0)
    {
        // OAM
        throw std::runtime_error("BUS READ NOT IMPLEMENTED");
    }

    if (addr < 0xFF00)
    {
        // reserved unusable
        return 0;
    }

    if (addr < 0xFF80)
    {
        // IO Registers
        throw std::runtime_error("BUS READ NOT IMPLEMENTED");
    }

    if (addr == 0XFFFF)
    {
        // Cpu enable register
        return cpu_->get_ie_register();
    }

    return ram_->hram_read(addr);
}

std::uint16_t Bus::read16(std::uint16_t addr)
{
    auto lo = read(addr);
    auto hi = read(addr + 1);

    return (hi << 8) | lo;
}

void Bus::write(std::uint16_t addr, std::uint8_t val)
{
    if (addr < 0x8000)
    {
        cart_->write(addr, val);
        return;
    }

    if (addr < 0xA000)
    {
        throw std::runtime_error("BUS WRITE NOT IMPLEMENTED");
    }

    if (addr < 0xC000)
    {
        // Cartridge ram
        cart_->write(addr, val);
        return;
    }

    if (addr < 0xE000)
    {
        // WRAM (Working RAM)
        ram_->wram_write(addr, val);
        return;
    }

    if (addr < 0xFE00)
    {
        // reserved echo ram
        return;
    }

    if (addr < 0xFEA0)
    {
        // OAM
        throw std::runtime_error("BUS WRITE NOT IMPLEMENTED");
    }

    if (addr < 0xFF00)
    {
        // reserved unusable
        return;
    }

    if (addr < 0xFF80)
    {
        // IO Registers
        // throw std::runtime_error("BUS WRITE NOT IMPLEMENTED");
        std::cout << "BUS WRITE NOT IMPLEMENTED\n";
        return;
    }

    if (addr == 0XFFFF)
    {
        // Cpu enable register
        cpu_->set_ie_register(val);
        return;
    }

    ram_->hram_write(addr, val);
}

void Bus::write16(std::uint16_t addr, std::uint16_t val)
{
    write(addr + 1, (val >> 8) & 0xFF);
    write(addr, val & 0xFF);
}

} // namespace game_boy