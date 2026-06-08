#pragma once

#include "bus.h"
#include "common.h"
#include "debug.h"
#include "instructions.h"

#include <cstdint>
#include <functional>

namespace game_boy
{

struct Registers
{
    std::uint8_t  a{0};
    std::uint8_t  f{0};
    std::uint8_t  b{0};
    std::uint8_t  c{0};
    std::uint8_t  d{0};
    std::uint8_t  e{0};
    std::uint8_t  h{0};
    std::uint8_t  l{0};
    std::uint16_t pc{0};
    std::uint16_t sp{0};
};

struct CpuContext
{
    Registers     regs{};
    std::uint16_t curr_fetch_data{0};
    std::uint16_t mem_dest{0};
    std::uint8_t  curr_opcode{0};
    bool          dest_is_mem{false};

    std::optional<Instruction> curr_instr;

    bool halted{false};
    bool stepping{false};

    bool         int_master_enabled{false};
    bool         enabling_ime{false};
    std::uint8_t ie_register{0};
    std::uint8_t int_flags{0};
};

enum InterruptType : uint8_t
{
    IT_VBLANK    = 1,
    IT_LCD_START = 2,
    IT_TIMER     = 4,
    IT_SERIAL    = 8,
    IT_JOYPAD    = 16,
};

class Cpu
{
  public:
    void init();
    bool step();

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

    void request_interrupt(InterruptType i);

    std::uint8_t int_flags();
    void         set_int_flags(std::uint8_t val);

  private:
    using IN_PROC = std::function<void()>;

    void          fetch_instruction();
    void          fetch_data();
    void          cpu_set_flags(int z, int n, int h, int c);
    void          execute();
    std::uint16_t read_register(RegisterType reg);
    void          set_register(RegisterType reg, std::uint16_t val);
    std::uint8_t  read_register8(RegisterType reg);
    void          set_register8(RegisterType reg, std::uint8_t val);

    static bool         is_16_bit(RegisterType rt);
    static RegisterType decode_reg(std::uint8_t reg);

    void handle_interrupts();
    void handle_interrupt(std::uint16_t addr);
    bool int_check(std::uint16_t addr, InterruptType it);

    auto flag_z() const { return BIT(context_.regs.f, 7); }
    auto flag_n() const { return BIT(context_.regs.f, 6); }
    auto flag_h() const { return BIT(context_.regs.f, 5); }
    auto flag_c() const { return BIT(context_.regs.f, 4); }

    bool check_cond();

    uint8_t get_zero_flag() const;
    uint8_t get_carry_flag() const;

    void goto_addr(std::uint16_t addr, bool pushpc);

    int        cycles_{0};
    CpuContext context_;
    Bus*       bus_;
    Debug      debug_;
    // map that maps instruction to function for processing them
    std::unordered_map<InstructionType, IN_PROC> instruction_processors_;
};
} // namespace game_boy