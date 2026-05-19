#include "ui.h"

#include "emu.h"

#include <iostream>

namespace game_boy
{

void UI::init()
{
    SDL_Init(SDL_INIT_VIDEO);
    std::cout << "SDL was initialized\n";
    TTF_Init();
    std::cout << "TTF was initialized\n";

    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    sdl_window_.reset(window);
    sdl_renderer_.reset(renderer);
}

void UI::insert_emu_context(EmuContext* context) { emu_context_ = context; }

void UI::delay(std::uint32_t ms) { SDL_Delay(ms); }

void UI::handle_events()
{
    SDL_Event event;

    while (SDL_PollEvent(&event) > 0)
    {
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
        {
            emu_context_->die = true;
        }
    }
}
} // namespace game_boy