#pragma once

#include "cart.h"
#include "cpu.h"

#include <cstdint>

namespace game_boy
{
struct EmuContext
{
    bool          paused;
    bool          running;
    std::uint64_t ticks;
};

class Emu
{
  public:
    Emu()                      = default;
    Emu(const Emu&)            = default;
    Emu(Emu&&)                 = delete;
    Emu& operator=(const Emu&) = delete;
    Emu& operator=(Emu&&)      = delete;

    [[nodiscard]] const EmuContext& get_context() const noexcept;
    void                            run(int argc, char** argv);
    static void                     delay(std::uint32_t ms);

  private:
    EmuContext context_;
    Cart       cart_;
    Cpu        cpu_;
};
} // namespace game_boy