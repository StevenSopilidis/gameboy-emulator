#include "ui.h"

#include "emu.h"

#include <iostream>

namespace
{
constexpr int                          SCALE       = 4;
constexpr std::array<unsigned long, 4> TILE_COLORS = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555,
                                                      0xFF000000};

} // namespace

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

    ////////////////////
    // debug sdl objects
    ////////////////////
    SDL_CreateWindowAndRenderer(16 * 8 * SCALE, 32 * 8 * SCALE, 0, &window, &renderer);

    sdl_debug_window_.reset(window);
    sdl_debug_renderer_.reset(renderer);

    debug_screen_.reset(SDL_CreateRGBSurface(0, (16 * 8 * SCALE) + (16 * SCALE),
                                             (32 * 8 * SCALE) + (64 * SCALE), 32, 0x00FF0000,
                                             0x0000FF00, 0x000000FF, 0xFF000000));

    sdl_debug_texture_.reset(SDL_CreateTexture(
        sdl_debug_renderer_.get(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
        (16 * 8 * SCALE) + (16 * 8), (32 * 8 * SCALE) + (64 * 8)));

    int x, y;
    SDL_GetWindowPosition(sdl_window_.get(), &x, &y);
    SDL_SetWindowPosition(sdl_debug_window_.get(), x + SCREEN_WIDTH + 10, y);
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

void UI::update() { updateDebugWindow(); }

void UI::updateDebugWindow()
{
    int xDraw{0};
    int yDraw{0};
    int tileNum{0};

    SDL_Rect rc;
    rc.x = 0;
    rc.y = 0;
    rc.w = debug_screen_->w;
    rc.h = debug_screen_->h;
    SDL_FillRect(debug_screen_.get(), &rc, 0xFF111111);

    std::uint16_t addr = 0x8000;

    for (std::size_t y{0}; y < 24; y++)
    {
        for (std::size_t x{0}; x < 24; x++)
        {
            display_tile(debug_screen_.get(), addr, tileNum, xDraw + (x * SCALE),
                         yDraw + (y * SCALE));
            xDraw += (8 * SCALE);
            tileNum++;
        }

        yDraw += (8 * SCALE);
        xDraw = 0;
    }

    SDL_UpdateTexture(sdl_debug_texture_.get(), nullptr, debug_screen_->pixels,
                      debug_screen_->pitch);
    SDL_RenderClear(sdl_debug_renderer_.get());
    SDL_RenderCopy(sdl_debug_renderer_.get(), sdl_debug_texture_.get(), nullptr, nullptr);
    SDL_RenderPresent(sdl_debug_renderer_.get());
}

void UI::display_tile(SDL_Surface* surface, std::uint16_t startLocation, std::uint16_t tileNum,
                      int x, int y)
{
    SDL_Rect rect;

    for (std::size_t tileY{0}; tileY < 16; tileY += 2)
    {
        auto b1 = Bus::get_instance()->read(startLocation + (tileNum * 16) + tileY);
        auto b2 = Bus::get_instance()->read(startLocation + (tileNum * 16) + tileY + 1);

        for (int bit{7}; bit >= 0; bit--)
        {
            std::uint8_t hi = !!(b1 & (1 << bit)) << 1;
            std::uint8_t lo = !!(b2 & (1 << bit));

            std::uint8_t color = hi | lo;

            rect.x = x + ((7 - bit) * SCALE);
            rect.y = y + (tileY / 2 * SCALE);
            rect.w = SCALE;
            rect.h = SCALE;

            SDL_FillRect(surface, &rect, TILE_COLORS[color]);
        }
    }
}

} // namespace game_boy