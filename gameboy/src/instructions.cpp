#include "instructions.h"

#include <array>
#include <iostream>

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

        arr[0x01] = {.type = IN_LD, .mode = AM_R_D16, .reg1 = RT_BC};
        arr[0x02] = {.type = IN_LD, .mode = AM_MR_R, .reg1 = RT_BC, .reg2 = RT_A};

        arr[0x05] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_B};
        arr[0x06] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_B};

        arr[0x08] = {.type = IN_LD, .mode = AM_A16_R, .reg2 = RT_SP};

        arr[0x0A] = {.type = IN_LD, .mode = AM_R_MR, .reg1 = RT_A, .reg2 = RT_BC};

        arr[0x0E] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_C};

        // 0x1X
        arr[0x11] = {.type = IN_LD, .mode = AM_R_D16, .reg1 = RT_DE};
        arr[0x12] = {.type = IN_LD, .mode = AM_MR_R, .reg1 = RT_DE, .reg2 = RT_A};
        arr[0x15] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_D};
        arr[0x16] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_D};
        arr[0x1A] = {.type = IN_LD, .mode = AM_R_MR, .reg1 = RT_A, .reg2 = RT_DE};
        arr[0x1E] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_E};

        // 0x2X
        arr[0x21] = {.type = IN_LD, .mode = AM_R_D16, .reg1 = RT_HL};
        arr[0x22] = {.type = IN_LD, .mode = AM_HLI_R, .reg1 = RT_HL, .reg2 = RT_A};
        arr[0x25] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_H};
        arr[0x26] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_H};
        arr[0x2A] = {.type = IN_LD, .mode = AM_R_HLI, .reg1 = RT_A, .reg2 = RT_HL};
        arr[0x2E] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_L};

        // 0x3X
        arr[0x31] = {.type = IN_LD, .mode = AM_R_D16, .reg1 = RT_SP};
        arr[0x32] = {.type = IN_LD, .mode = AM_HLD_R, .reg1 = RT_HL, .reg2 = RT_A};
        arr[0x35] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_HL};
        arr[0x36] = {.type = IN_LD, .mode = AM_MR_D8, .reg1 = RT_HL};
        arr[0x3A] = {.type = IN_LD, .mode = AM_R_HLD, .reg1 = RT_A, .reg2 = RT_HL};
        arr[0x3E] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_A};

        // 0x4X
        arr[0x40] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_B, .reg2 = RT_B};
        arr[0x41] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_B, .reg2 = RT_C};
        arr[0x42] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_B, .reg2 = RT_D};
        arr[0x43] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_B, .reg2 = RT_E};
        arr[0x44] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_B, .reg2 = RT_H};
        arr[0x45] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_B, .reg2 = RT_L};
        arr[0x46] = {.type = IN_LD, .mode = AM_R_MR, .reg1 = RT_B, .reg2 = RT_HL};
        arr[0x47] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_B, .reg2 = RT_A};

        arr[0x48] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_C, .reg2 = RT_B};
        arr[0x49] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_C, .reg2 = RT_C};
        arr[0x4A] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_C, .reg2 = RT_D};
        arr[0x4B] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_C, .reg2 = RT_E};
        arr[0x4C] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_C, .reg2 = RT_H};
        arr[0x4D] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_C, .reg2 = RT_L};
        arr[0x4E] = {.type = IN_LD, .mode = AM_R_MR, .reg1 = RT_C, .reg2 = RT_HL};
        arr[0x4F] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_C, .reg2 = RT_A};

        // 0x5X
        arr[0x50] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_D, .reg2 = RT_B};
        arr[0x51] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_D, .reg2 = RT_C};
        arr[0x52] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_D, .reg2 = RT_D};
        arr[0x53] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_D, .reg2 = RT_E};
        arr[0x54] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_D, .reg2 = RT_H};
        arr[0x55] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_D, .reg2 = RT_L};
        arr[0x56] = {.type = IN_LD, .mode = AM_R_MR, .reg1 = RT_D, .reg2 = RT_HL};
        arr[0x57] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_D, .reg2 = RT_A};

        arr[0x58] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_E, .reg2 = RT_B};
        arr[0x59] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_E, .reg2 = RT_C};
        arr[0x5A] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_E, .reg2 = RT_D};
        arr[0x5B] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_E, .reg2 = RT_E};
        arr[0x5C] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_E, .reg2 = RT_H};
        arr[0x5D] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_E, .reg2 = RT_L};
        arr[0x5E] = {.type = IN_LD, .mode = AM_R_MR, .reg1 = RT_E, .reg2 = RT_HL};
        arr[0x5F] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_E, .reg2 = RT_A};

        // 0x6X
        arr[0x60] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_H, .reg2 = RT_B};
        arr[0x61] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_H, .reg2 = RT_C};
        arr[0x62] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_H, .reg2 = RT_D};
        arr[0x63] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_H, .reg2 = RT_E};
        arr[0x64] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_H, .reg2 = RT_H};
        arr[0x65] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_H, .reg2 = RT_L};
        arr[0x66] = {.type = IN_LD, .mode = AM_R_MR, .reg1 = RT_H, .reg2 = RT_HL};
        arr[0x67] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_H, .reg2 = RT_A};

        arr[0x68] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_L, .reg2 = RT_B};
        arr[0x69] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_L, .reg2 = RT_C};
        arr[0x6A] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_L, .reg2 = RT_D};
        arr[0x6B] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_L, .reg2 = RT_E};
        arr[0x6C] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_L, .reg2 = RT_H};
        arr[0x6D] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_L, .reg2 = RT_L};
        arr[0x6E] = {.type = IN_LD, .mode = AM_R_MR, .reg1 = RT_L, .reg2 = RT_HL};
        arr[0x6F] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_L, .reg2 = RT_A};

        // 0x7X
        arr[0x70] = {.type = IN_LD, .mode = AM_MR_R, .reg1 = RT_HL, .reg2 = RT_B};
        arr[0x71] = {.type = IN_LD, .mode = AM_MR_R, .reg1 = RT_HL, .reg2 = RT_C};
        arr[0x72] = {.type = IN_LD, .mode = AM_MR_R, .reg1 = RT_HL, .reg2 = RT_D};
        arr[0x73] = {.type = IN_LD, .mode = AM_MR_R, .reg1 = RT_HL, .reg2 = RT_E};
        arr[0x74] = {.type = IN_LD, .mode = AM_MR_R, .reg1 = RT_HL, .reg2 = RT_H};
        arr[0x75] = {.type = IN_LD, .mode = AM_MR_R, .reg1 = RT_HL, .reg2 = RT_L};

        arr[0x76] = {.type = IN_HALT, .mode = AM_IMP};

        arr[0x77] = {.type = IN_LD, .mode = AM_MR_R, .reg1 = RT_HL, .reg2 = RT_A};

        arr[0x78] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_B};
        arr[0x79] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_C};
        arr[0x7A] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_D};
        arr[0x7B] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_E};
        arr[0x7C] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_H};
        arr[0x7D] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_L};
        arr[0x7E] = {.type = IN_LD, .mode = AM_R_MR, .reg1 = RT_A, .reg2 = RT_HL};
        arr[0x7F] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_A};

        arr[0xAF] = {.type = IN_XOR, .mode = AM_R, .reg1 = RT_A};

        arr[0xC3] = {.type = IN_JP, .mode = AM_D16};

        // 0xEX
        arr[0xE2] = {.type = IN_LD, .mode = AM_MR_R, .reg1 = RT_C, .reg2 = RT_A};
        arr[0xEA] = {.type = IN_LD, .mode = AM_A16_R, .reg2 = RT_A};

        // 0xFX
        arr[0xF2] = {.type = IN_LD, .mode = AM_R_MR, .reg1 = RT_A, .reg2 = RT_C};
        arr[0xF3] = {.type = IN_DI, .mode = AM_IMP};
        arr[0xFA] = {.type = IN_LD, .mode = AM_R_A16, .reg1 = RT_A};

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