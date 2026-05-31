#pragma once

#include <cstdint>
namespace game_boy
{
struct TimerContext
{
    std::uint16_t div;
    std::uint8_t  tima;
    std::uint8_t  tma;
    std::uint8_t  tac;
};

class Cpu;

class Timer
{
  public:
    void init();
    void tick();

    void         write(std::uint16_t addr, std::uint8_t val);
    std::uint8_t read(std::uint16_t addr);

    void connect_cpu(Cpu* cpu);

    static Timer& get_instance();

    [[nodiscard]] TimerContext* context();

  private:
    Timer() = default;

    Cpu*         cpu_;
    TimerContext context_;
};
} // namespace game_boy