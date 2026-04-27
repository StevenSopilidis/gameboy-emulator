#pragma once

#include "bus.h"
#include "instructions.h"

#include <cstdint>
#include <functional>

namespace game_boy
{
struct Registers
{
    std::uint8_t  a;
    std::uint8_t  f;
    std::uint8_t  b;
    std::uint8_t  c;
    std::uint8_t  d;
    std::uint8_t  e;
    std::uint8_t  h;
    std::uint8_t  l;
    std::uint16_t pc;
    std::uint16_t sp;
};

struct CpuContext
{
    Registers     regs;
    std::uint16_t curr_fetch_data;
    std::uint16_t mem_dest;
    std::uint8_t  curr_opcode;
    bool          dest_is_mem;

    std::optional<Instruction> curr_instr;

    bool halted;
    bool stepping;

    bool         int_master_enabled;
    std::uint8_t ie_register;
};

class Cpu
{
  public:
    void init();
    bool step();

    void set_bus(Bus* bus);

    void              inc_cycles(int cpu_cycles) noexcept;
    [[nodiscard]] int get_cycles() const noexcept;

    [[nodiscard]] std::uint8_t get_ie_register() const noexcept;
    void                       set_ie_register(std::uint8_t val) noexcept;

    [[nodiscard]] const Registers* get_regs() const;

    // stack operations
    void          stack_push(std::uint8_t data);
    void          stack_push16(std::uint16_t data);
    std::uint8_t  stack_pop();
    std::uint16_t stack_pop16();

  private:
    using IN_PROC = std::function<void()>;

    void          fetch_instruction();
    void          fetch_data();
    void          cpu_set_flags(char z, char n, char h, char c);
    void          execute();
    std::uint16_t read_register(RegisterType reg);
    void          set_register(RegisterType reg, std::uint16_t val);

    bool check_cond();

    uint8_t get_zero_flag() const;
    uint8_t get_carry_flag() const;

    void goto_addr(std::uint16_t addr, bool pushpc);

    int        cycles_{0};
    CpuContext context_;
    Bus*       bus_;
    // map that maps instruction to function for processing them
    std::unordered_map<InstructionType, IN_PROC> instruction_processors_;
};
} // namespace game_boy