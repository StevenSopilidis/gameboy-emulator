#include "emu.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <format>
#include <iostream>
#include <stdexcept>

namespace game_boy
{
const EmuContext& Emu::get_context() const noexcept { return context_; }

void Emu::delay(std::uint32_t ms) { SDL_Delay(ms); }

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

    SDL_Init(SDL_INIT_VIDEO);
    std::cout << "SDL was initialized\n";
    TTF_Init();
    std::cout << "TTF was initialized\n";

    cpu_.init();
    cpu_.set_bus(&bus_);
    bus_.insert_cpu(&cpu_);

    context_.running = true;
    context_.paused  = false;
    context_.ticks   = 0;

    while (context_.running)
    {
        if (context_.paused)
        {
            delay(10);
            continue;
        }

        if (!cpu_.step())
        {
            throw std::runtime_error("Cpu stopped");
        }

        context_.ticks++;
    }
}

} // namespace game_boy