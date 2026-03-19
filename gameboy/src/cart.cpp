#include "cart.h"

#include <format>
#include <fstream>
#include <iostream>

namespace game_boy
{

constexpr std::array<std::string_view, 35> ROM_TYPES = {
    "ROM ONLY",
    "MBC1",
    "MBC1+RAM",
    "MBC1+RAM+BATTERY",
    "0x04 ???",
    "MBC2",
    "MBC2+BATTERY",
    "0x07 ???",
    "ROM+RAM 1",
    "ROM+RAM+BATTERY 1",
    "0x0A ???",
    "MMM01",
    "MMM01+RAM",
    "MMM01+RAM+BATTERY",
    "0x0E ???",
    "MBC3+TIMER+BATTERY",
    "MBC3+TIMER+RAM+BATTERY 2",
    "MBC3",
    "MBC3+RAM 2",
    "MBC3+RAM+BATTERY 2",
    "0x14 ???",
    "0x15 ???",
    "0x16 ???",
    "0x17 ???",
    "0x18 ???",
    "MBC5",
    "MBC5+RAM",
    "MBC5+RAM+BATTERY",
    "MBC5+RUMBLE",
    "MBC5+RUMBLE+RAM",
    "MBC5+RUMBLE+RAM+BATTERY",
    "0x1F ???",
    "MBC6",
    "0x21 ???",
    "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
};

constexpr std::array<std::string_view, 0xA5> LIC_CODE = []
{
    std::array<std::string_view, 0xA5> arr{};

    arr[0x00] = "None";
    arr[0x01] = "Nintendo R&D1";
    arr[0x08] = "Capcom";
    arr[0x13] = "Electronic Arts";
    arr[0x18] = "Hudson Soft";
    arr[0x19] = "b-ai";
    arr[0x20] = "kss";
    arr[0x22] = "pow";
    arr[0x24] = "PCM Complete";
    arr[0x25] = "san-x";
    arr[0x28] = "Kemco Japan";
    arr[0x29] = "seta";
    arr[0x30] = "Viacom";
    arr[0x31] = "Nintendo";
    arr[0x32] = "Bandai";
    arr[0x33] = "Ocean/Acclaim";
    arr[0x34] = "Konami";
    arr[0x35] = "Hector";
    arr[0x37] = "Taito";
    arr[0x38] = "Hudson";
    arr[0x39] = "Banpresto";
    arr[0x41] = "Ubi Soft";
    arr[0x42] = "Atlus";
    arr[0x44] = "Malibu";
    arr[0x46] = "angel";
    arr[0x47] = "Bullet-Proof";
    arr[0x49] = "irem";
    arr[0x50] = "Absolute";
    arr[0x51] = "Acclaim";
    arr[0x52] = "Activision";
    arr[0x53] = "American sammy";
    arr[0x54] = "Konami";
    arr[0x55] = "Hi tech entertainment";
    arr[0x56] = "LJN";
    arr[0x57] = "Matchbox";
    arr[0x58] = "Mattel";
    arr[0x59] = "Milton Bradley";
    arr[0x60] = "Titus";
    arr[0x61] = "Virgin";
    arr[0x64] = "LucasArts";
    arr[0x67] = "Ocean";
    arr[0x69] = "Electronic Arts";
    arr[0x70] = "Infogrames";
    arr[0x71] = "Interplay";
    arr[0x72] = "Broderbund";
    arr[0x73] = "sculptured";
    arr[0x75] = "sci";
    arr[0x78] = "THQ";
    arr[0x79] = "Accolade";
    arr[0x80] = "misawa";
    arr[0x83] = "lozc";
    arr[0x86] = "Tokuma Shoten Intermedia";
    arr[0x87] = "Tsukuda Original";
    arr[0x91] = "Chunsoft";
    arr[0x92] = "Video system";
    arr[0x93] = "Ocean/Acclaim";
    arr[0x95] = "Varie";
    arr[0x96] = "Yonezawa/s’pal";
    arr[0x97] = "Kaneko";
    arr[0x99] = "Pack in soft";
    arr[0xA4] = "Konami (Yu-Gi-Oh!)";

    return arr;
}();

Cart::Cart() : context_{std::make_unique<CartContext>()} {}

std::string_view Cart::license_name() const noexcept
{
    if (context_->header->new_lic_code <= 0xA4)
    {
        return LIC_CODE[context_->header->new_lic_code];
    }

    return "UNKNOWN";
}

std::string_view Cart::rom_type() const noexcept
{
    if (context_->header->type <= 0x22)
    {
        return ROM_TYPES[context_->header->type];
    }

    return "UNKNOWN";
}

bool Cart::load(std::string_view cart)
{
    context_->filename = cart;

    std::ifstream file((std::string(cart)));
    if (!file)
    {
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streampos size = file.tellg(); // get position

    context_->rom_size = size;

    file.seekg(0, std::ios::beg); // back to start

    context_->rom_data.reserve(context_->rom_size);

    if (!file.read(reinterpret_cast<char*>(context_->rom_data.data()), size))
    {
        return false;
    }

    context_->header            = (RomHeader*)(context_->rom_data.data() + 0x100);
    context_->header->title[15] = 0;

    auto* title    = context_->header->title.data();
    auto  type_str = rom_type();
    auto  lic_str  = license_name();

    std::uint16_t x = 0;
    for (std::uint16_t i = 0x0134; i <= 0x014C; i++)
    {
        x = x - context_->rom_data[i] - 1;
    }

    uint8_t     checksum     = context_->header->checksum;
    bool        check_passed = (x & 0xFF) != 0; // your original logic
    const auto* check_status = check_passed ? "PASSED" : "FAILED";

    std::cout << std::format("Cartridge Loaded:\n"
                             "\t Title    : {}\n"
                             "\t Type     : {:02X} ({})\n"
                             "\t ROM Size : {} KB\n"
                             "\t RAM Size : {:02X}\n"
                             "\t LIC Code : {:02X} ({})\n"
                             "\t ROM Vers : {:02X}\n"
                             "\t Checksum : {:02X} ({})\n",
                             title, context_->header->type, type_str,
                             32 << context_->header->rom_size, context_->header->ram_size,
                             context_->header->lic_code, lic_str, context_->header->version,
                             checksum, check_status);

    return true;
}

} // namespace game_boy