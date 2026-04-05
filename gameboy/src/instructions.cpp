#include "instructions.h"

#include <array>

namespace game_boy
{
constexpr std::array<std::string_view, 48> instr_lookup{
    "<NONE>", "NOP",    "LD",     "INC",     "DEC",    "RLCA",   "ADD",    "RRCA",
    "STOP",   "RLA",    "JR",     "RRA",     "DAA",    "CPL",    "SCF",    "CCF",
    "HALT",   "ADC",    "SUB",    "SBC",     "AND",    "XOR",    "OR",     "CP",
    "POP",    "JP",     "PUSH",   "RET",     "CB",     "CALL",   "RETI",   "LDH",
    "JPHL",   "DI",     "EI",     "RST",     "IN_ERR", "IN_RLC", "IN_RRC", "IN_RL",
    "IN_RR",  "IN_SLA", "IN_SRA", "IN_SWAP", "IN_SRL", "IN_BIT", "IN_RES", "IN_SET"};

auto get_instruction = [](std::size_t index)
{
    static const std::array<Instruction, 0x100> instructions = []
    {
        std::array<Instruction, 0x100> arr{};

        arr[0x00] = {.type = IN_NOP, .mode = AM_IMP};
        arr[0x05] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_B};
        arr[0x0E] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_C};
        arr[0xAF] = {.type = IN_XOR, .mode = AM_R, .reg1 = RT_A};
        arr[0xC3] = {.type = IN_JP, .mode = AM_D16};
        arr[0xF3] = {.type = IN_DI, .mode = AM_IMP};

        return arr;
    }();

    return instructions[index];
};

std::optional<Instruction> instruction_by_opcode(std::uint8_t opcode)
{
    if (get_instruction(opcode).type == IN_NONE)
    {
        return std::nullopt;
    }

    return get_instruction(opcode);
}

std::string_view get_instruction_name(InstructionType instr) { return instr_lookup[instr]; }

} // namespace game_boy