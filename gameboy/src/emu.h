#pragma once

#include "bus.h"
#include "cart.h"
#include "cpu.h"
#include "ppu.h"
#include "ram.h"
#include "timer.h"
#include "ui.h"

#include <cstdint>
#include <thread>

namespace game_boy
{

struct EmuContext
{
    bool          paused{false};
    bool          running{false};
    bool          die{false};
    std::uint64_t ticks{0};
};

class Emu
{
  public:
    Emu();
    Emu(const Emu&)            = delete;
    Emu(Emu&&)                 = delete;
    Emu& operator=(const Emu&) = delete;
    Emu& operator=(Emu&&)      = delete;

    [[nodiscard]] const EmuContext& get_context() const noexcept;
    void                            run(int argc, char** argv);
    void                            run_cpu();

    void emu_cycles(int cpu_cycles);

  private:
    EmuContext context_;
    Cart       cart_;
    UI         ui_;
    Ram        ram_;
    Ppu        ppu_;
    Cpu        cpu_;
    Bus*       bus_;

    std::thread cpu_thread_;
};
} // namespace game_boy