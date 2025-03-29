#pragma once

#ifndef CHIP8_CONSTANTS_H
#define CHIP8_CONSTANTS_H 1

#include <cstdint>

namespace chip8::constants
{
    template<typename T>
    using C8Const = inline constexpr T;

    namespace memory
    {
        C8Const<std::uint16_t> MEMORY_SIZE      = (1024 * 4);
        C8Const<std::uint16_t> FONTSET_SIZE     = 80;
        C8Const<std::uint16_t> UNUSED_SIZE      = 138;
        C8Const<std::uint16_t> MAPPINGS_SIZE    = 294;
        C8Const<std::uint16_t> INTERPRETER_SIZE =
            FONTSET_SIZE + MAPPINGS_SIZE + UNUSED_SIZE;
    }

    namespace display
    {
        C8Const<std::uint16_t> WIDTH  = 64;
        C8Const<std::uint16_t> HEIGHT = WIDTH / 2;
        C8Const<std::uint16_t> BUFFER_SIZE = (WIDTH * HEIGHT) / 8;
    }

    namespace registers
    {
        C8Const<std::uint16_t> DATA_REGISTER_SIZE   = 16; /// V[0-F]
        C8Const<std::uint16_t> NUM_FLOW_REGISTERS   = 2;  /// I, PC
        C8Const<std::uint16_t> NUM_TIMER_REGISTERS  = 2;  /// DT, ST
        C8Const<std::uint16_t> STACK_SIZE           = 16;
    }
}

#endif // !CHIP8_CONSTANTS_H

