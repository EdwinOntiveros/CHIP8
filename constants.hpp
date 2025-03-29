#pragma once

#ifndef CHIP8_CONSTANTS_H
#define CHIP8_CONSTANTS_H 1

#include <cstdint>

namespace chip8::constants
{
    namespace memory
    {
        inline constexpr std::uint16_t MEMORY_SIZE      = (1024 * 4);
        inline constexpr std::uint16_t FONTSET_SIZE     = 80;
        inline constexpr std::uint16_t UNUSED_SIZE      = 138;
        inline constexpr std::uint16_t MAPPINGS_SIZE    = 294;
        inline constexpr std::uint16_t INTERPRETER_SIZE =
            FONTSET_SIZE + MAPPINGS_SIZE + UNUSED_SIZE;
    }

    namespace display
    {
        inline constexpr  std::uint16_t WIDTH  = 64;
        inline constexpr  std::uint16_t HEIGHT = WIDTH / 2;
        inline constexpr  std::uint16_t BUFFER_SIZE = (WIDTH * HEIGHT) / 8;
    }

    namespace registers
    {
        inline constexpr  std::uint16_t DATA_REGISTER_SIZE   = 16; /// V[0-F]
        inline constexpr  std::uint16_t NUM_FLOW_REGISTERS   = 2;  /// I, PC
        inline constexpr  std::uint16_t NUM_TIMER_REGISTERS  = 2;  /// DT, ST
        inline constexpr  std::uint16_t STACK_SIZE          = 16;
    }

    namespace fontset
    {
        inline constexpr std::uint16_t NUM_FONT_SPRITES = 16;
    }
}

#endif // !CHIP8_CONSTANTS_H

