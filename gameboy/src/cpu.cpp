#include "cpu.h"

#include "common.h"
#include "instructions.h"

#include <format>
#include <iostream>

namespace game_boy
{
std::uint16_t reverse(uint16_t n) { return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8); }

uint8_t Cpu::get_zero_flag() const { return BIT(context_.regs.f, 7); }

uint8_t Cpu::get_carry_flag() const { return BIT(context_.regs.f, 4); }

bool Cpu::check_cond(CpuContext* context)
{
    auto z = get_zero_flag();
    auto c = get_carry_flag();

    switch (context->curr_instr->cond)
    {
    case CT_NONE:
        return true;
    case CT_C:
        return c;
    case CT_NC:
        return !c;
    case CT_Z:
        return z;
    case CT_NZ:
        return !z;
    }

    return false;
}

void Cpu::cpu_set_flags(char z, char n, char h, char c)
{
    if (z != -1)
    {
        BIT_SET(context_.regs.f, 7, z);
    }

    if (n != -1)
    {
        BIT_SET(context_.regs.f, 6, n);
    }

    if (h != -1)
    {
        BIT_SET(context_.regs.f, 5, h);
    }

    if (c != -1)
    {
        BIT_SET(context_.regs.f, 4, c);
    }
}

void Cpu::init()
{
    context_.regs.pc = 0x100;
    context_.regs.a  = 0x01;

    instruction_processors_[IN_NOP] = []() {};

    instruction_processors_[IN_NONE] = []() { throw std::runtime_error("Invalid instruction\n"); };

    instruction_processors_[IN_LD] = []() {};
    instruction_processors_[IN_JP] = [&]()
    {
        if (check_cond(&context_))
        {
            context_.regs.pc = context_.curr_fetch_data;
            inc_cycles(1);
        }
    };

    instruction_processors_[IN_DI] = [&]() { context_.int_master_enabled = false; };

    instruction_processors_[IN_XOR] = [&]()
    {
        context_.regs.a ^= context_.curr_fetch_data & 0Xff;
        cpu_set_flags((context_.regs.a == 0), 0, 0, 0);
    };
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

void Cpu::set_register(RegisterType reg, std::uint16_t val)
{
    switch (reg)
    {
    case RT_A:
        context_.regs.a = val & 0xFF;
        break;
    case RT_F:
        context_.regs.f = val & 0xFF;
        break;
    case RT_B:
        context_.regs.b = val & 0xFF;
        break;
    case RT_C:
        context_.regs.c = val & 0xFF;
        break;
    case RT_D:
        context_.regs.d = val & 0xFF;
        break;
    case RT_E:
        context_.regs.e = val & 0xFF;
        break;
    case RT_H:
        context_.regs.h = val & 0xFF;
        break;
    case RT_L:
        context_.regs.l = val & 0xFF;
        break;
    case RT_AF:
        *((std::uint16_t*)&context_.regs.a) = reverse(val);
        break;
    case RT_BC:
        *((std::uint16_t*)&context_.regs.b) = reverse(val);
        break;
    case RT_DE:
        *((std::uint16_t*)&context_.regs.d) = reverse(val);
        break;
    case RT_HL:
        *((std::uint16_t*)&context_.regs.h) = reverse(val);
        break;
    case RT_PC:
        context_.regs.pc = val;
        break;
    case RT_SP:
        context_.regs.sp = val;
        break;
    case RT_NONE:
        break;
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
    case AM_R_R:
        context_.curr_fetch_data = read_register(context_.curr_instr->reg2);
        return;
    case AM_R_D8:
        context_.curr_fetch_data = bus_->read(context_.regs.pc);
        inc_cycles(1);
        context_.regs.pc++;
        return;
    case AM_R_D16:
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
    case AM_MR_R:
        context_.curr_fetch_data = read_register(context_.curr_instr->reg2);
        context_.mem_dest        = read_register(context_.curr_instr->reg1);
        context_.dest_is_mem     = true;

        if (context_.curr_instr->reg1 == RT_C)
        {
            context_.mem_dest |= 0xFF00;
        }
        return;
    case AM_R_MR:
    {
        auto addr = read_register(context_.curr_instr->reg2);
        if (context_.curr_instr->reg2 == RT_C)
        {
            addr |= 0xFF00;
        }
        context_.curr_fetch_data = bus_->read(addr);
        inc_cycles(1);
        return;
    }
    case AM_R_HLI:
    {
        context_.curr_fetch_data = bus_->read(read_register(context_.curr_instr->reg2));
        inc_cycles(1);
        set_register(RT_HL, read_register(RT_HL) + 1);
        return;
    }
    case AM_R_HLD:
    {
        context_.curr_fetch_data = bus_->read(read_register(context_.curr_instr->reg2));
        inc_cycles(1);
        set_register(RT_HL, read_register(RT_HL) - 1);
        return;
    }
    case AM_HLI_R:
    {
        context_.curr_fetch_data = read_register(context_.curr_instr->reg2);
        context_.mem_dest        = read_register(context_.curr_instr->reg1);
        context_.dest_is_mem     = true;
        set_register(RT_HL, read_register(RT_HL) + 1);
        return;
    }
    case AM_HLD_R:
    {
        context_.curr_fetch_data = read_register(context_.curr_instr->reg2);
        context_.mem_dest        = read_register(context_.curr_instr->reg1);
        context_.dest_is_mem     = true;
        set_register(RT_HL, read_register(RT_HL) - 1);
        return;
    }
    case AM_R_A8:
        context_.curr_fetch_data = bus_->read(context_.regs.pc);
        inc_cycles(1);
        context_.regs.pc++;
        return;
    case AM_R_A16:
    {
        auto lo = bus_->read(context_.regs.pc);
        inc_cycles(1);

        auto hi = bus_->read(context_.regs.pc + 1);
        inc_cycles(1);

        auto addr = lo | (hi << 8);

        context_.regs.pc += 2;
        context_.curr_fetch_data = bus_->read(addr);
        inc_cycles(1);
        return;
    }
    case AM_A8_R:
        context_.mem_dest    = bus_->read(context_.regs.pc) | 0xFF00;
        context_.dest_is_mem = true;
        inc_cycles(1);
        context_.regs.pc++;
        return;
    case AM_HL_SPR:
        context_.curr_fetch_data = bus_->read(context_.regs.pc);
        inc_cycles(1);
        context_.regs.pc++;
        return;
    case AM_D8:
        context_.curr_fetch_data = bus_->read(context_.regs.pc);
        inc_cycles(1);
        context_.regs.pc++;
        return;
    case AM_A16_R:
    case AM_D16_R:
    {
        std::uint16_t lo = bus_->read(context_.regs.pc);
        inc_cycles(1);
        std::uint16_t hi = bus_->read(context_.regs.pc + 1);
        inc_cycles(1);

        context_.mem_dest    = static_cast<std::uint16_t>(lo | (hi << 8));
        context_.dest_is_mem = true;

        context_.regs.pc += 2;
        context_.curr_fetch_data = read_register(context_.curr_instr->reg2);
        return;
    }
    case AM_MR:
        context_.mem_dest        = read_register(context_.curr_instr->reg1);
        context_.dest_is_mem     = true;
        context_.curr_fetch_data = bus_->read(read_register(context_.curr_instr->reg1));
        inc_cycles(1);
        return;
    case AM_MR_D8:
        context_.curr_fetch_data = bus_->read(context_.regs.pc);
        inc_cycles(1);
        context_.regs.pc++;

        context_.mem_dest    = read_register(context_.curr_instr->reg1);
        context_.dest_is_mem = true;
        return;
    default:
        throw std::runtime_error(&"Unknown addressing mode: "[context_.curr_instr->mode]);
    }
}

void Cpu::execute()
{
    auto it = instruction_processors_.find(context_.curr_instr->type);

    if (it == instruction_processors_.end())
    {
        throw std::runtime_error("Instruction not yet implemented");
    }

    it->second();
}

bool Cpu::step()
{
    if (!context_.halted)
    {
        auto pc = context_.regs.pc;
        fetch_instruction();
        fetch_data();

        std::cout << std::format(
            "{:04X}: {} ({:02X} {:02X} {:02X}), A: {:02X} B: {:02X} C: {:02X}\n", pc,
            get_instruction_name(context_.curr_instr->type), context_.curr_opcode,
            bus_->read(pc + 1), bus_->read(pc + 2), context_.regs.a, context_.regs.b,
            context_.regs.c);

        execute();
    }

    return true;
}
} // namespace game_boy