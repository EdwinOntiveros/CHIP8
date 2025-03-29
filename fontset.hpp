#pragma once

#ifndef CHIP8_FONTSET_H
#define CHIP8_FONTSET_H 1

#include <cstdint>
#include "constants.hpp"
#include "types.hpp"

namespace fontset
{
    using namespace chip8;
    using namespace types::array_types;
    using namespace constants::fontset;

    inline constexpr DWordArray<fontset::NUM_FONT_SPRITES> compressed_sprites {
        0xF999F,  // 0x00
        0x26227,  // 0x01
        0xF1F8F,  // 0x02
        0xF1F1F,  // 0x03
        0x99F11,  // 0x04
        0xF8F1F,  // 0x05
        0xF8F9F,  // 0x06
        0xF1244,  // 0x07
        0xF9F9F,  // 0x08
        0xF9F1F,  // 0x09
        0xF9F99,  // 0x0A
        0xF999F,  // 0x0B
        0xE9E9E,  // 0x0C
        0xF8888,  // 0x0D
        0xF8F8F,  // 0x0E
        0xF8F88   // 0x0F
    };

}

#endif // CHIP8_FONTSET_H

