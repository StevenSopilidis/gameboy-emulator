#pragma once

#include "bus.h"
#include "instructions.h"

#include <cstdint>
#include <memory>

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
};

class Cpu
{
  public:
    void init();
    bool step();

    void set_bus(Bus* bus);

    void              inc_cycles(int cpu_cycles) noexcept;
    [[nodiscard]] int get_cycles() const noexcept;

    void          fetch_instruction();
    void          fetch_data();
    void          execute();
    std::uint16_t read_register(RegisterType reg);

  private:
    int        cycles_{0};
    CpuContext context_;
    Bus*       bus_; 
};
} // namespace game_boy