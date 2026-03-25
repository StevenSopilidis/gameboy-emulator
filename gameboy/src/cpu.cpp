#include "cpu.h"

#include "instructions.h"

#include <iostream>

namespace game_boy
{
std::uint16_t reverse(uint16_t n) { return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8); }

void Cpu::init()
{
    context_.regs.pc = 0x100;
    context_.regs.a  = 0x01;
}

void Cpu::set_bus(Bus* bus) { bus_ = bus; }

void Cpu::fetch_instruction()
{
    context_.curr_opcode = bus_->read(context_.regs.pc++);
    context_.curr_instr  = instruction_by_opcode(context_.curr_opcode);

    if (context_.curr_instr == std::nullopt)
    {
        throw std::runtime_error("Unknown instruction fetched");
    }
}

void Cpu::inc_cycles(int cpu_cycles) noexcept {}

int Cpu::get_cycles() const noexcept { return cycles_; }

std::uint16_t Cpu::read_register(RegisterType reg)
{
    switch (reg)
    {
    case RT_A:
        return context_.regs.a;
    case RT_F:
        return context_.regs.f;
    case RT_B:
        return context_.regs.b;
    case RT_C:
        return context_.regs.c;
    case RT_D:
        return context_.regs.d;
    case RT_E:
        return context_.regs.e;
    case RT_H:
        return context_.regs.h;
    case RT_L:
        return context_.regs.l;

    case RT_AF:
        return reverse(*((std::uint16_t*)&context_.regs.a));
    case RT_BC:
        return reverse(*((std::uint16_t*)&context_.regs.b));
    case RT_DE:
        return reverse(*((std::uint16_t*)&context_.regs.d));
    case RT_HL:
        return reverse(*((std::uint16_t*)&context_.regs.h));

    case RT_PC:
        return context_.regs.pc;
    case RT_SP:
        return context_.regs.sp;
    default:
        return 0;
    }
}

void Cpu::fetch_data()
{
    context_.mem_dest    = 0;
    context_.dest_is_mem = false;

    switch (context_.curr_instr->mode)
    {
    case AM_IMP:
        return;
    case AM_R:
        context_.curr_fetch_data = read_register(context_.curr_instr->reg1);
        return;
    case AM_R_D8:
        context_.curr_fetch_data = bus_->read(context_.regs.pc);
        inc_cycles(1);
        context_.regs.pc++;
        return;
    case AM_D16:
    {
        std::uint16_t lo = bus_->read(context_.regs.pc);
        inc_cycles(1);
        std::uint16_t hi = bus_->read(context_.regs.pc + 1);
        inc_cycles(1);
        context_.curr_fetch_data = static_cast<std::uint16_t>(lo | (hi << 8));
        context_.regs.pc += 2;
        return;
    }
    default:
        throw std::runtime_error(&"Unknown addressing mode: "[context_.curr_instr->mode]);
    }
}

void Cpu::execute() { throw std::runtime_error("execute() not yet implemented"); }

bool Cpu::step()
{
    if (!context_.halted)
    {
        auto pc = context_.regs.pc;
        fetch_instruction();
        fetch_data();
        printf("Executing instruction: %02X  PC: %04x \n", context_.curr_opcode, pc);
        execute();
    }

    return false;
}
} // namespace game_boy