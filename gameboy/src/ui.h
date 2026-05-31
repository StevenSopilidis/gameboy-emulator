#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <common.h>
#include <memory>

namespace game_boy
{
class EmuContext;

struct SDLWindowDeleter
{
    void operator()(SDL_Window* w) const
    {
        if (w)
        {
            SDL_DestroyWindow(w);
        }
    }
};

struct SDLRendererDeleter
{
    void operator()(SDL_Renderer* r) const
    {
        if (r)
        {
            SDL_DestroyRenderer(r);
        }
    }
};

struct SDLTextureDeleter
{
    void operator()(SDL_Texture* t) const
    {
        if (t)
        {
            SDL_DestroyTexture(t);
        }
    }
};

struct SDLSurfaceDeleter
{
    void operator()(SDL_Surface* s) const
    {
        if (s)
        {
            SDL_FreeSurface(s);
        }
    }
};

class UI
{
  public:
    void init();
    void handle_events();

    void insert_emu_context(EmuContext* context);

    void delay(std::uint32_t ms);

  private:
    static const int SCREEN_WIDTH{1024};
    static const int SCREEN_HEIGHT{768};

    std::unique_ptr<SDL_Window, SDLWindowDeleter>     sdl_window_;
    std::unique_ptr<SDL_Renderer, SDLRendererDeleter> sdl_renderer_;
    std::unique_ptr<SDL_Texture, SDLTextureDeleter>   sdl_texture_;
    std::unique_ptr<SDL_Surface, SDLSurfaceDeleter>   sdl_surface_;

    EmuContext* emu_context_;
};

} // namespace game_boy