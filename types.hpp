#pragma once

#ifndef CHIP8_TYPES_H
#define CHIP8_TYPES_H 1

#include <cstdint>
#include <array>

namespace chip8::types
{
    namespace array_types
    {
        template<std::size_t size>
        using ByteArray = std::array<std::uint8_t, size>;

        template<std::size_t size>
        using WordArray = std::array<std::uint16_t, size>;

        template<std::size_t size>
        using DWordArray = std::array<std::uint32_t, size>;
    }
}

#endif // !CHIP8_TYPES_H

