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

bool Cpu::check_cond()
{
    auto z = get_zero_flag();
    auto c = get_carry_flag();

    switch (context_.curr_instr->cond)
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

    instruction_processors_[IN_LD] = [&]()
    {
        if (context_.dest_is_mem)
        {
            if (Cpu::is_16_bit(context_.curr_instr->reg2))
            {
                // 16 bit register
                inc_cycles(1);
                bus_->write16(context_.mem_dest, context_.curr_fetch_data);
            }
            else
            {
                bus_->write(context_.mem_dest, context_.curr_fetch_data);
            }

            inc_cycles(1);
            return;
        }

        if (context_.curr_instr->mode == AM_HL_SPR)
        {
            std::uint8_t hflag = (read_register(context_.curr_instr->reg2) & 0xF) +
                                     (context_.curr_fetch_data & 0xF) >=
                                 0x10;

            std::uint8_t cflag = (read_register(context_.curr_instr->reg2) & 0xFF) +
                                     (context_.curr_fetch_data & 0xFF) >=
                                 0x100;

            cpu_set_flags(0, 0, hflag, cflag);
            set_register(context_.curr_instr->reg1,
                         read_register(context_.curr_instr->reg2) + (char)context_.curr_fetch_data);

            return;
        }

        set_register(context_.curr_instr->reg1, context_.curr_fetch_data);
    };

    instruction_processors_[IN_JP] = [&]() { goto_addr(context_.curr_fetch_data, false); };

    instruction_processors_[IN_CALL] = [&]() { goto_addr(context_.curr_fetch_data, true); };

    instruction_processors_[IN_RST] = [&]() { goto_addr(context_.curr_instr->param, true); };

    instruction_processors_[IN_RET] = [&]()
    {
        if (context_.curr_instr->cond != CT_NONE)
        {
            inc_cycles(1);
        }

        if (check_cond())
        {
            std::uint16_t addr = stack_pop16();
            context_.regs.pc   = addr;

            inc_cycles(1);
        }
    };

    instruction_processors_[IN_RETI] = [&]()
    {
        context_.int_master_enabled = true;
        instruction_processors_[IN_RET]();
    };

    instruction_processors_[IN_JR] = [&]()
    {
        auto          rel  = (char)(context_.curr_fetch_data & 0xFF);
        std::uint16_t addr = context_.regs.pc + rel;
        goto_addr(addr, false);
    };

    instruction_processors_[IN_DI] = [&]() { context_.int_master_enabled = false; };

    instruction_processors_[IN_XOR] = [&]()
    {
        context_.regs.a ^= context_.curr_fetch_data & 0Xff;
        cpu_set_flags((context_.regs.a == 0), 0, 0, 0);
    };

    instruction_processors_[IN_LDH] = [&]()
    {
        if (context_.curr_instr->reg1 == RT_A)
        {
            set_register(context_.curr_instr->reg1, bus_->read(0xFF00 | context_.curr_fetch_data));
        }
        else
        {
            bus_->write(0xFF00 | context_.curr_fetch_data, context_.regs.a);
        }

        inc_cycles(1);
    };

    instruction_processors_[IN_PUSH] = [&]()
    {
        auto hi = (read_register(context_.curr_instr->reg1) >> 8) & 0xFF;
        stack_push(hi);

        auto lo = read_register(context_.curr_instr->reg1) & 0xFF;
        stack_push(lo);

        inc_cycles(1);
    };

    instruction_processors_[IN_POP] = [&]()
    {
        auto data = stack_pop16();
        set_register(context_.curr_instr->reg1, data);

        if (context_.curr_instr->reg1 == RT_AF)
        {
            set_register(context_.curr_instr->reg1, data & 0xFFF0);
        }
    };

    instruction_processors_[IN_INC] = [&]()
    {
        auto val = read_register(context_.curr_instr->reg1) + 1;

        if (Cpu::is_16_bit(context_.curr_instr->reg1))
        {
            inc_cycles(1);
        }

        if (context_.curr_instr->reg1 == RT_HL && context_.curr_instr->mode == AM_MR)
        {
            val = bus_->read(read_register(RT_HL)) + 1;
            val &= 0xFF;
            bus_->write(read_register(RT_HL), val);
        }
        else
        {
            set_register(context_.curr_instr->reg1, val);
            val = read_register(context_.curr_instr->reg1);
        }

        if ((context_.curr_opcode & 0x03) == 0x03)
        {
            return;
        }

        cpu_set_flags(val == 0, 0, (val & 0x0F) == 0, -1);
    };

    instruction_processors_[IN_DEC] = [&]()
    {
        std::uint16_t val = read_register(context_.curr_instr->reg1) - 1;

        if (Cpu::is_16_bit(context_.curr_instr->reg1))
        {
            inc_cycles(1);
        }

        if (context_.curr_instr->reg1 == RT_HL && context_.curr_instr->mode == AM_MR)
        {
            val = bus_->read(read_register(RT_HL)) - 1;
            bus_->write(read_register(RT_HL), val);
        }
        else
        {
            set_register(context_.curr_instr->reg1, val);
            val = read_register(context_.curr_instr->reg1);
        }

        if ((context_.curr_opcode & 0x0B) == 0x0B)
        {
            return;
        }

        cpu_set_flags(val == 0, 1, (val & 0x0F) == 0x0f, -1);
    };

    instruction_processors_[IN_ADD] = [&]()
    {
        std::uint32_t val = read_register(context_.curr_instr->reg1) + context_.curr_fetch_data;

        auto is_16bit = is_16_bit(context_.curr_instr->reg1);
        if (is_16bit)
        {
            inc_cycles(1);
        }

        if (context_.curr_instr->reg1 == RT_SP)
        {
            val = read_register(context_.curr_instr->reg1) + (char)context_.curr_fetch_data;
        }

        int z = (val & 0xFF) == 0;
        int h =
            (read_register(context_.curr_instr->reg1) & 0xF) + (context_.curr_fetch_data & 0xF) >=
            0x10;
        int c = (int)(read_register(context_.curr_instr->reg1) & 0xFF) +
                    (int)(context_.curr_fetch_data & 0xFF) >=
                0x100;

        if (is_16bit)
        {
            z = -1;
            h = (read_register(context_.curr_instr->reg1) & 0xFFF) +
                    (context_.curr_fetch_data & 0xFFF) >=
                0x1000;
            auto n = ((std::uint32_t)read_register(context_.curr_instr->reg1)) +
                     ((std::uint32_t)context_.curr_fetch_data);
            c = n >= 0x10000;
        }

        if (context_.curr_instr->reg1 == RT_SP)
        {
            z = 0;
            h = (read_register(context_.curr_instr->reg1) & 0xF) +
                    (context_.curr_fetch_data & 0xF) >=
                0x10;
            c = (int)(read_register(context_.curr_instr->reg1) & 0xFF) +
                    (int)(context_.curr_fetch_data & 0xFF) >=
                0x100;
        }

        set_register(context_.curr_instr->reg1, val & 0xFFFF);
        cpu_set_flags(z, 0, h, c);
    };

    instruction_processors_[IN_ADC] = [&]()
    {
        auto u = context_.curr_fetch_data;
        auto a = context_.regs.a;
        auto c = flag_c();

        context_.regs.a = (a + u + c) & 0xFF;
        cpu_set_flags(context_.regs.a == 0, 0, (a & 0xF) + (u & 0xF) + c > 0xF, a + u + c > 0xFF);
    };

    instruction_processors_[IN_SUB] = [&]()
    {
        auto val = read_register(context_.curr_instr->reg1) - context_.curr_fetch_data;

        int z = val == 0;
        int h = ((int)read_register(context_.curr_instr->reg1) & 0xF) -
                    ((int)context_.curr_fetch_data & 0xF) <
                0;

        int c =
            ((int)read_register(context_.curr_instr->reg1)) - ((int)context_.curr_fetch_data) < 0;

        set_register(context_.curr_instr->reg1, val);
        cpu_set_flags(z, 1, h, c);
    };

    instruction_processors_[IN_SBC] = [&]()
    {
        std::uint8_t val = context_.curr_fetch_data + get_carry_flag();

        int z = read_register(context_.curr_instr->reg1) - val == 0;
        int h = ((int)read_register(context_.curr_instr->reg1) & 0xF) -
                    ((int)context_.curr_fetch_data & 0xF) - ((int)get_carry_flag()) <
                0;

        int c = ((int)read_register(context_.curr_instr->reg1)) - ((int)context_.curr_fetch_data) -
                    ((int)get_carry_flag()) <
                0;

        set_register(context_.curr_instr->reg1, read_register(context_.curr_instr->reg1) - val);
        cpu_set_flags(z, 1, h, c);
    };
}

bool Cpu::is_16_bit(RegisterType rt) { return rt >= RegisterType::RT_AF; }

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

        std::array<char, 16> flags;
        sprintf(flags.data(), "%c%c%c%c", context_.regs.f & (1 << 7) ? 'Z' : '-',
                context_.regs.f & (1 << 6) ? 'N' : '-', context_.regs.f & (1 << 5) ? 'H' : '-',
                context_.regs.f & (1 << 4) ? 'C' : '-');

        std::cout << std::format(
            "{:04X}: {} ({:02X} {:02X} {:02X}), A: {:02X} F: {} BC: {:02X}{:02X} "
            "DE: {:02X}{:02X} HL: {:02X}{:02X}\n",
            pc, get_instruction_name(context_.curr_instr->type), context_.curr_opcode,
            bus_->read(pc + 1), bus_->read(pc + 2), context_.regs.a, flags.data(), context_.regs.b,
            context_.regs.c, context_.regs.d, context_.regs.e, context_.regs.h, context_.regs.l);

        execute();
    }

    return true;
}

std::uint8_t Cpu::get_ie_register() const noexcept { return context_.ie_register; }

void Cpu::set_ie_register(std::uint8_t val) noexcept { context_.ie_register = val; }

const Registers* Cpu::get_regs() const { return &context_.regs; }

void Cpu::stack_push(std::uint8_t data)
{
    context_.regs.sp--;
    bus_->write(context_.regs.sp, data);
    inc_cycles(1);
}

void Cpu::stack_push16(std::uint16_t data)
{
    stack_push((data >> 8) & 0xFF);
    stack_push(data & 0xFF);
}

std::uint8_t Cpu::stack_pop()
{
    auto data = bus_->read(context_.regs.sp++);
    inc_cycles(1);
    return data;
}

std::uint16_t Cpu::stack_pop16()
{
    auto lo = stack_pop();
    auto hi = stack_pop();

    return (hi << 8) | lo;
}

void Cpu::goto_addr(std::uint16_t addr, bool pushpc)
{
    if (check_cond())
    {
        if (pushpc)
        {
            stack_push16(context_.regs.pc);
        }

        context_.regs.pc = addr;
        inc_cycles(1);
    }
}

} // namespace game_boy