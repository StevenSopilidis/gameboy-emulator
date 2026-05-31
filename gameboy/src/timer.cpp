#include "timer.h"

#include "cpu.h"

namespace game_boy
{

void Timer::init() { context_.div = 0xAC00; }

void Timer::tick()
{
    auto prev_div = context_.div;
    context_.div++;

    auto timer_update{false};

    switch (context_.tac & (0b11))
    {
    case 0b00:
        timer_update = (prev_div & (1 << 9) && !(context_.div & (1 << 9)));
        break;
    case 0b01:
        timer_update = (prev_div & (1 << 3) && !(context_.div & (1 << 3)));
        break;
    case 0b10:
        timer_update = (prev_div & (1 << 5) && !(context_.div & (1 << 5)));
        break;
    case 0b11:
        timer_update = (prev_div & (1 << 7) && !(context_.div & (1 << 7)));
        break;
    }

    if (timer_update && (context_.tac & (1 << 2)))
    {
        context_.tima++;

        if (context_.tima == 0xFF)
        {
            context_.tima = context_.tma;

            cpu_->request_interrupt(IT_TIMER);
        }
    }
}

Timer& Timer::get_instance()
{
    static Timer timer;

    return timer;
}

void Timer::connect_cpu(Cpu* cpu) { cpu_ = cpu; }

void Timer::write(std::uint16_t addr, std::uint8_t val)
{
    switch (addr)
    {
    case 0xFF04:
        // DIV
        context_.div = 0;
        break;
    case 0xFF05:
        // TIMA
        context_.tima = val;
        break;
    case 0xFF06:
        // TMA
        context_.tma = val;
        break;
    case 0xFF07:
        // TAC
        context_.tac = val;
        break;
    }
}

std::uint8_t Timer::read(std::uint16_t addr)
{
    switch (addr)
    {
    case 0xFF04:
        // DIV
        return context_.div >> 8;
    case 0xFF05:
        // TIMA
        return context_.tima;
    case 0xFF06:
        // TMA
        return context_.tma;
    case 0xFF07:
        // TAC
        return context_.tac;
    }

    return 0;
}

[[nodiscard]] TimerContext* Timer::context() { return &context_; }

} // namespace game_boy