#pragma once

#include <array>
#include <cstdint>

namespace game_boy
{

// object attribute entry
struct OamEntry
{
    std::uint8_t y;
    std::uint8_t x;
    std::uint8_t tile_index;

    // flags
    unsigned f_cgb_pn : 3;
    unsigned f_cgb_vram_bank : 1;
    unsigned f_pn : 1;
    unsigned f_x_flip : 1;
    unsigned f_y_flip : 1;
    unsigned f_bgp : 1;
};

struct PpuContext
{
    std::array<OamEntry, 40>         oam_ram;
    std::array<std::uint8_t, 0x2000> vram;
};

class Ppu
{
  public:
    void init();
    void tick();

    void         oam_write(std::uint16_t addr, std::uint8_t value);
    std::uint8_t oam_read(std::uint16_t addr);

    void         vram_write(std::uint16_t addr, std::uint8_t value);
    std::uint8_t vram_read(std::uint16_t addr);

  private:
    PpuContext context_;
};

} // namespace game_boy