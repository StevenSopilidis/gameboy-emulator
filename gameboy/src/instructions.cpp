#include "instructions.h"

#include "bus.h"
#include "cpu.h"

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

constexpr std::array<std::string_view, 15> rt_lookups = {
    "<NONE>", "A", "F", "B", "C", "D", "E", "H", "L", "AF", "BC", "DE", "HL", "SP", "PC"};

auto get_instruction = [](std::size_t index)
{
    static const std::array<Instruction, 0x100> instructions = []
    {
        std::array<Instruction, 0x100> arr{};

        arr[0x00] = {.type = IN_NOP, .mode = AM_IMP};

        arr[0x01] = {.type = IN_LD, .mode = AM_R_D16, .reg1 = RT_BC};
        arr[0x02] = {.type = IN_LD, .mode = AM_MR_R, .reg1 = RT_BC, .reg2 = RT_A};
        arr[0x03] = {.type = IN_INC, .mode = AM_R, .reg1 = RT_BC};
        arr[0x04] = {.type = IN_INC, .mode = AM_R, .reg1 = RT_B};

        arr[0x05] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_B};
        arr[0x06] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_B};
        arr[0x07] = {.type = IN_RLCA};
        arr[0x08] = {.type = IN_LD, .mode = AM_A16_R, .reg2 = RT_SP};
        arr[0x09] = {.type = IN_ADD, .mode = AM_R_R, .reg1 = RT_HL, .reg2 = RT_BC};

        arr[0x0A] = {.type = IN_LD, .mode = AM_R_MR, .reg1 = RT_A, .reg2 = RT_BC};
        arr[0x0B] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_BC};
        arr[0x0C] = {.type = IN_INC, .mode = AM_R, .reg1 = RT_C};
        arr[0x0D] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_C};

        arr[0x0E] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_C};
        arr[0x0F] = {.type = IN_RRCA};

        // 0x1X
        arr[0x10] = {.type = IN_STOP};
        arr[0x11] = {.type = IN_LD, .mode = AM_R_D16, .reg1 = RT_DE};
        arr[0x12] = {.type = IN_LD, .mode = AM_MR_R, .reg1 = RT_DE, .reg2 = RT_A};
        arr[0x13] = {.type = IN_INC, .mode = AM_R, .reg1 = RT_DE};
        arr[0x14] = {.type = IN_INC, .mode = AM_R, .reg1 = RT_D};
        arr[0x15] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_D};
        arr[0x16] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_D};
        arr[0x17] = {.type = IN_RLA};
        arr[0x18] = {.type = IN_JR, .mode = AM_D8};
        arr[0x19] = {.type = IN_ADD, .mode = AM_R_R, .reg1 = RT_HL, .reg2 = RT_DE};
        arr[0x1A] = {.type = IN_LD, .mode = AM_R_MR, .reg1 = RT_A, .reg2 = RT_DE};
        arr[0x1B] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_DE};
        arr[0x1C] = {.type = IN_INC, .mode = AM_R, .reg1 = RT_E};
        arr[0x1D] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_E};
        arr[0x1E] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_E};
        arr[0x1F] = {.type = IN_RRA};

        // 0x2X
        arr[0x20] = {.type = IN_JR, .mode = AM_D8, .cond = CT_NZ};
        arr[0x21] = {.type = IN_LD, .mode = AM_R_D16, .reg1 = RT_HL};
        arr[0x22] = {.type = IN_LD, .mode = AM_HLI_R, .reg1 = RT_HL, .reg2 = RT_A};
        arr[0x23] = {.type = IN_INC, .mode = AM_R, .reg1 = RT_HL};
        arr[0x24] = {.type = IN_INC, .mode = AM_R, .reg1 = RT_H};
        arr[0x25] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_H};
        arr[0x26] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_H};
        arr[0x27] = {.type = IN_DAA};
        arr[0x28] = {.type = IN_JR, .mode = AM_D8, .cond = CT_Z};
        arr[0x29] = {.type = IN_ADD, .mode = AM_R_R, .reg1 = RT_HL, .reg2 = RT_HL};
        arr[0x2A] = {.type = IN_LD, .mode = AM_R_HLI, .reg1 = RT_A, .reg2 = RT_HL};
        arr[0x2B] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_HL};
        arr[0x2C] = {.type = IN_INC, .mode = AM_R, .reg1 = RT_L};
        arr[0x2D] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_L};
        arr[0x2E] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_L};
        arr[0x2F] = {.type = IN_CPL};

        // 0x3X
        arr[0x30] = {.type = IN_JR, .mode = AM_D8, .cond = CT_NC};
        arr[0x31] = {.type = IN_LD, .mode = AM_R_D16, .reg1 = RT_SP};
        arr[0x32] = {.type = IN_LD, .mode = AM_HLD_R, .reg1 = RT_HL, .reg2 = RT_A};
        arr[0x33] = {.type = IN_INC, .mode = AM_R, .reg1 = RT_SP};
        arr[0x34] = {.type = IN_INC, .mode = AM_MR, .reg1 = RT_HL};
        arr[0x35] = {.type = IN_DEC, .mode = AM_MR, .reg1 = RT_HL};
        arr[0x36] = {.type = IN_LD, .mode = AM_MR_D8, .reg1 = RT_HL};
        arr[0x37] = {.type = IN_SCF};
        arr[0x38] = {.type = IN_JR, .mode = AM_D8, .cond = CT_C};
        arr[0x39] = {.type = IN_ADD, .mode = AM_R_R, .reg1 = RT_HL, .reg2 = RT_SP};
        arr[0x3A] = {.type = IN_LD, .mode = AM_R_HLD, .reg1 = RT_A, .reg2 = RT_HL};
        arr[0x3B] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_SP};
        arr[0x3C] = {.type = IN_INC, .mode = AM_R, .reg1 = RT_A};
        arr[0x3D] = {.type = IN_DEC, .mode = AM_R, .reg1 = RT_A};
        arr[0x3E] = {.type = IN_LD, .mode = AM_R_D8, .reg1 = RT_A};
        arr[0x3F] = {.type = IN_CCF};

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

        // 0x8x
        arr[0x80] = {.type = IN_ADD, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_B};
        arr[0x81] = {.type = IN_ADD, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_C};
        arr[0x82] = {.type = IN_ADD, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_D};
        arr[0x83] = {.type = IN_ADD, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_E};
        arr[0x84] = {.type = IN_ADD, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_H};
        arr[0x85] = {.type = IN_ADD, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_L};
        arr[0x86] = {.type = IN_ADD, .mode = AM_R_MR, .reg1 = RT_A, .reg2 = RT_HL};
        arr[0x87] = {.type = IN_ADD, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_A};
        arr[0x88] = {.type = IN_ADC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_B};
        arr[0x89] = {.type = IN_ADC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_C};
        arr[0x8A] = {.type = IN_ADC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_D};
        arr[0x8B] = {.type = IN_ADC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_E};
        arr[0x8C] = {.type = IN_ADC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_H};
        arr[0x8D] = {.type = IN_ADC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_L};
        arr[0x8E] = {.type = IN_ADC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_HL};
        arr[0x8F] = {.type = IN_ADC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_A};

        // 0x9x
        arr[0x90] = {.type = IN_SUB, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_B};
        arr[0x91] = {.type = IN_SUB, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_C};
        arr[0x92] = {.type = IN_SUB, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_D};
        arr[0x93] = {.type = IN_SUB, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_E};
        arr[0x94] = {.type = IN_SUB, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_H};
        arr[0x95] = {.type = IN_SUB, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_L};
        arr[0x96] = {.type = IN_SUB, .mode = AM_R_MR, .reg1 = RT_A, .reg2 = RT_HL};
        arr[0x97] = {.type = IN_SUB, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_A};
        arr[0x98] = {.type = IN_SBC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_B};
        arr[0x99] = {.type = IN_SBC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_C};
        arr[0x9A] = {.type = IN_SBC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_D};
        arr[0x9B] = {.type = IN_SBC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_E};
        arr[0x9C] = {.type = IN_SBC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_H};
        arr[0x9D] = {.type = IN_SBC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_L};
        arr[0x9E] = {.type = IN_SBC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_HL};
        arr[0x9F] = {.type = IN_SBC, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_A};

        // 0xAx
        arr[0xA0] = {.type = IN_AND, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_B};
        arr[0xA1] = {.type = IN_AND, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_C};
        arr[0xA2] = {.type = IN_AND, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_D};
        arr[0xA3] = {.type = IN_AND, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_E};
        arr[0xA4] = {.type = IN_AND, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_H};
        arr[0xA5] = {.type = IN_AND, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_L};
        arr[0xA6] = {.type = IN_AND, .mode = AM_R_MR, .reg1 = RT_A, .reg2 = RT_HL};
        arr[0xA7] = {.type = IN_AND, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_A};
        arr[0xA8] = {.type = IN_XOR, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_B};
        arr[0xA9] = {.type = IN_XOR, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_C};
        arr[0xAA] = {.type = IN_XOR, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_D};
        arr[0xAB] = {.type = IN_XOR, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_E};
        arr[0xAC] = {.type = IN_XOR, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_H};
        arr[0xAD] = {.type = IN_XOR, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_L};
        arr[0xAE] = {.type = IN_XOR, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_HL};
        arr[0xAF] = {.type = IN_XOR, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_A};

        // 0xBx
        arr[0xB0] = {.type = IN_OR, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_B};
        arr[0xB1] = {.type = IN_OR, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_C};
        arr[0xB2] = {.type = IN_OR, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_D};
        arr[0xB3] = {.type = IN_OR, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_E};
        arr[0xB4] = {.type = IN_OR, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_H};
        arr[0xB5] = {.type = IN_OR, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_L};
        arr[0xB6] = {.type = IN_OR, .mode = AM_R_MR, .reg1 = RT_A, .reg2 = RT_HL};
        arr[0xB7] = {.type = IN_OR, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_A};
        arr[0xB8] = {.type = IN_CP, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_B};
        arr[0xB9] = {.type = IN_CP, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_C};
        arr[0xBA] = {.type = IN_CP, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_D};
        arr[0xBB] = {.type = IN_CP, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_E};
        arr[0xBC] = {.type = IN_CP, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_H};
        arr[0xBD] = {.type = IN_CP, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_L};
        arr[0xBE] = {.type = IN_CP, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_HL};
        arr[0xBF] = {.type = IN_CP, .mode = AM_R_R, .reg1 = RT_A, .reg2 = RT_A};

        // 0xCx
        arr[0xC0] = {.type = IN_RET, .mode = AM_IMP, .cond = CT_NZ};
        arr[0xC1] = {.type = IN_POP, .mode = AM_R, .reg1 = RT_BC};
        arr[0xC2] = {.type = IN_JP, .mode = AM_D16, .cond = CT_NZ};
        arr[0xC3] = {.type = IN_JP, .mode = AM_D16};
        arr[0xC4] = {.type = IN_CALL, .mode = AM_D16, .cond = CT_NZ};
        arr[0xC5] = {.type = IN_PUSH, .mode = AM_R, .reg1 = RT_BC};
        arr[0xC6] = {.type = IN_ADD, .mode = AM_R_D8, .reg1 = RT_A};
        arr[0xC7] = {.type = IN_RST, .mode = AM_IMP, .param = 0x00};
        arr[0xC8] = {.type = IN_RET, .mode = AM_IMP, .cond = CT_Z};
        arr[0xC9] = {.type = IN_RET};
        arr[0xCA] = {.type = IN_JP, .mode = AM_D16, .cond = CT_Z};
        arr[0xCB] = {.type = IN_CB, .mode = AM_D8};
        arr[0xCC] = {.type = IN_CALL, .mode = AM_D16, .cond = CT_Z};
        arr[0xCD] = {.type = IN_CALL, .mode = AM_D16};
        arr[0xCE] = {.type = IN_ADC, .mode = AM_R_D8, .reg1 = RT_A};
        arr[0xCF] = {.type = IN_RST, .mode = AM_IMP, .param = 0x08};

        // 0xDX
        arr[0xD0] = {.type = IN_RET, .mode = AM_IMP, .cond = CT_NC};
        arr[0xD1] = {.type = IN_POP, .mode = AM_R, .reg1 = RT_DE};
        arr[0xD2] = {.type = IN_JP, .mode = AM_D16, .cond = CT_NC};
        arr[0xD4] = {.type = IN_CALL, .mode = AM_D16, .cond = CT_NC};
        arr[0xD5] = {.type = IN_PUSH, .mode = AM_R, .reg1 = RT_DE};
        arr[0xD6] = {.type = IN_SUB, .mode = AM_R_D8, .reg1 = RT_A};
        arr[0xD7] = {.type = IN_RST, .mode = AM_IMP, .param = 0x10};
        arr[0xD8] = {.type = IN_RET, .mode = AM_IMP, .cond = CT_C};
        arr[0xD9] = {.type = IN_RETI};
        arr[0xDA] = {.type = IN_JP, .mode = AM_D16, .cond = CT_C};
        arr[0xDC] = {.type = IN_CALL, .mode = AM_D16, .cond = CT_C};
        arr[0xDE] = {.type = IN_SBC, .mode = AM_R_D8, .reg1 = RT_A};
        arr[0xDF] = {.type = IN_RST, .mode = AM_IMP, .param = 0x18};

        // 0xEX
        arr[0xE0] = {.type = IN_LDH, .mode = AM_A8_R, .reg1 = RT_NONE, .reg2 = RT_A};
        arr[0xE1] = {.type = IN_POP, .mode = AM_R, .reg1 = RT_HL};
        arr[0xE2] = {.type = IN_LD, .mode = AM_MR_R, .reg1 = RT_C, .reg2 = RT_A};
        arr[0xE5] = {.type = IN_PUSH, .mode = AM_R, .reg1 = RT_HL};
        arr[0xE6] = {.type = IN_AND, .mode = AM_R_D8, .reg1 = RT_A};
        arr[0xE7] = {.type = IN_RST, .mode = AM_IMP, .param = 0x20};
        arr[0xE8] = {.type = IN_ADD, .mode = AM_R_D8, .reg1 = RT_SP};
        arr[0xE9] = {.type = IN_JP, .mode = AM_R, .reg1 = RT_HL};
        arr[0xEA] = {.type = IN_LD, .mode = AM_A16_R, .reg2 = RT_A};
        arr[0xEE] = {.type = IN_XOR, .mode = AM_R_D8, .reg1 = RT_A};
        arr[0xEF] = {.type = IN_RST, .mode = AM_IMP, .param = 0x28};

        // 0xFX
        arr[0xF0] = {.type = IN_LDH, .mode = AM_R_A8, .reg1 = RT_A};
        arr[0xF1] = {.type = IN_POP, .mode = AM_R, .reg1 = RT_AF};
        arr[0xF2] = {.type = IN_LD, .mode = AM_R_MR, .reg1 = RT_A, .reg2 = RT_C};
        arr[0xF3] = {.type = IN_DI, .mode = AM_IMP};
        arr[0xF5] = {.type = IN_PUSH, .mode = AM_R, .reg1 = RT_AF};
        arr[0xF6] = {.type = IN_OR, .mode = AM_R_D8, .reg1 = RT_A};
        arr[0xF7] = {.type = IN_RST, .mode = AM_IMP, .param = 0x30};
        arr[0xF8] = {.type = IN_LD, .mode = AM_HL_SPR, .reg1 = RT_HL, .reg2 = RT_SP};
        arr[0xF9] = {.type = IN_LD, .mode = AM_R_R, .reg1 = RT_SP, .reg2 = RT_HL};
        arr[0xFA] = {.type = IN_LD, .mode = AM_R_A16, .reg1 = RT_A};
        arr[0xFB] = {.type = IN_EI};
        arr[0xFE] = {.type = IN_CP, .mode = AM_R_D8, .reg1 = RT_A};
        arr[0xFF] = {.type = IN_RST, .mode = AM_IMP, .param = 0x38};

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

void inst_to_str(CpuContext* ctx, Bus* bus, std::string& str)
{
    auto inst = ctx->curr_instr;
    sprintf(str.data(), "%s ", get_instruction_name(inst->type).data());

    switch (inst->mode)
    {
    case AM_IMP:
        return;

    case AM_R_D16:
    case AM_R_A16:
        sprintf(str.data(), "%s %s,$%04X", get_instruction_name(inst->type).data(),
                rt_lookups[inst->reg1].data(), ctx->curr_fetch_data);
        return;

    case AM_R:
        sprintf(str.data(), "%s %s", get_instruction_name(inst->type).data(),
                rt_lookups[inst->reg1].data());
        return;

    case AM_R_R:
        sprintf(str.data(), "%s %s,%s", get_instruction_name(inst->type).data(),
                rt_lookups[inst->reg1].data(), rt_lookups[inst->reg2].data());
        return;

    case AM_MR_R:
        sprintf(str.data(), "%s (%s),%s", get_instruction_name(inst->type).data(),
                rt_lookups[inst->reg1].data(), rt_lookups[inst->reg2].data());
        return;

    case AM_MR:
        sprintf(str.data(), "%s (%s)", get_instruction_name(inst->type).data(),
                rt_lookups[inst->reg1].data());
        return;

    case AM_R_MR:
        sprintf(str.data(), "%s %s,(%s)", get_instruction_name(inst->type).data(),
                rt_lookups[inst->reg1].data(), rt_lookups[inst->reg2].data());
        return;

    case AM_R_D8:
    case AM_R_A8:
        sprintf(str.data(), "%s %s,$%02X", get_instruction_name(inst->type).data(),
                rt_lookups[inst->reg1].data(), ctx->curr_fetch_data & 0xFF);
        return;

    case AM_R_HLI:
        sprintf(str.data(), "%s %s,(%s+)", get_instruction_name(inst->type).data(),
                rt_lookups[inst->reg1].data(), rt_lookups[inst->reg2].data());
        return;

    case AM_R_HLD:
        sprintf(str.data(), "%s %s,(%s-)", get_instruction_name(inst->type).data(),
                rt_lookups[inst->reg1].data(), rt_lookups[inst->reg2].data());
        return;

    case AM_HLI_R:
        sprintf(str.data(), "%s (%s+),%s", get_instruction_name(inst->type).data(),
                rt_lookups[inst->reg1].data(), rt_lookups[inst->reg2].data());
        return;

    case AM_HLD_R:
        sprintf(str.data(), "%s (%s-),%s", get_instruction_name(inst->type).data(),
                rt_lookups[inst->reg1].data(), rt_lookups[inst->reg2].data());
        return;

    case AM_A8_R:
        sprintf(str.data(), "%s $%02X,%s", get_instruction_name(inst->type).data(),
                bus->read(ctx->regs.pc - 1), rt_lookups[inst->reg2].data());

        return;

    case AM_HL_SPR:
        sprintf(str.data(), "%s (%s),SP+%d", get_instruction_name(inst->type).data(),
                rt_lookups[inst->reg1].data(), ctx->curr_fetch_data & 0xFF);
        return;

    case AM_D8:
        sprintf(str.data(), "%s $%02X", get_instruction_name(inst->type).data(),
                ctx->curr_fetch_data & 0xFF);
        return;

    case AM_D16:
        sprintf(str.data(), "%s $%04X", get_instruction_name(inst->type).data(),
                ctx->curr_fetch_data);
        return;

    case AM_MR_D8:
        sprintf(str.data(), "%s (%s),$%02X", get_instruction_name(inst->type).data(),
                rt_lookups[inst->reg1].data(), ctx->curr_fetch_data & 0xFF);
        return;

    case AM_A16_R:
        sprintf(str.data(), "%s ($%04X),%s", get_instruction_name(inst->type).data(),
                ctx->curr_fetch_data, rt_lookups[inst->reg2].data());
        return;

    default:
        fprintf(stderr, "INVALID AM: %d\n", inst->mode);
        throw std::runtime_error("Invalid adddresing mode");
    }
}

} // namespace game_boy