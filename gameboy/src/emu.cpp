#include "emu.h"

#include "timer.h"

#include <format>
#include <iostream>
#include <stdexcept>

namespace game_boy
{
const EmuContext& Emu::get_context() const noexcept { return context_; }

void Emu::run_cpu()
{
    cpu_.init();
    Timer::get_instance().connect_cpu(&cpu_);
    cpu_.set_bus(&bus_);
    bus_.insert_cpu(&cpu_);

    context_.running = true;
    context_.paused  = false;
    context_.ticks   = 0;

    ui_.insert_emu_context(&context_);

    while (context_.running)
    {
        if (context_.paused)
        {
            continue;
        }

        if (!cpu_.step())
        {
            throw std::runtime_error("Cpu stopped");
        }
    }
}

void Emu::run(int argc, char** argv)
{
    if (argc < 2)
    {
        throw std::runtime_error("Usage: emu <rom_file>\n");
    }

    if (!cart_.load(argv[1]))
    {
        throw std::runtime_error(std::format("Failed to load cart: {}", argv[1]));
    }

    bus_.insert_cart(&cart_);
    bus_.insert_ram(&ram_);

    std::cout << "Card loaded\n";

    cpu_thread_ = std::thread([&]() { run_cpu(); });

    ui_.init();

    while (!context_.die)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
        ui_.handle_events();
    }
}

void Emu::emu_cycles(int cpu_cycles)
{
    int n = cpu_cycles * 4;

    for (int i{0}; i < n; i++)
    {
        context_.ticks++;
        Timer::get_instance().tick();
    }
}

} // namespace game_boy