#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace game_boy
{

struct RomHeader
{
    std::array<std::uint8_t, 4>    entry;
    std::array<std::uint8_t, 0x30> logo;

    std::array<char, 16> title;
    std::uint16_t        new_lic_code;
    std::uint8_t         sgb_flag;
    std::uint8_t         type;
    std::uint8_t         rom_size;
    std::uint8_t         ram_size;
    std::uint8_t         dest_code;
    std::uint8_t         lic_code;
    std::uint8_t         version;
    std::uint8_t         checksum;
    std::uint16_t        global_checksum;
};

struct CartContext
{
    std::string               filename;
    unsigned long             rom_size;
    std::vector<std::uint8_t> rom_data;
    RomHeader*                header;
};

// constexpr std::string_view get_licensee(uint8_t code)
// {
//     return code < LIC_CODE.size() && !LIC_CODE[code].empty() ? LIC_CODE[code] : "Unknown";
// }

class Cart
{
  public:
    Cart();

    bool                           load(std::string_view cart);
    [[nodiscard]] std::string_view license_name() const noexcept;
    [[nodiscard]] std::string_view rom_type() const noexcept;

    std::uint8_t read(std::uint16_t addr);
    void         write(std::uint16_t addr, std::uint8_t val);

  private:
    std::unique_ptr<CartContext> context_;
};

} // namespace game_boy