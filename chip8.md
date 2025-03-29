# CHIP8 Emulator developer diary

[Original documentation](http://devernay.free.fr/hacks/chip8)

## Memory layout

    Chip 8 total memory: 4kb (4096 bytes)
    Addressable space:                       0x0000 - 0x0FFF (0 to 4095) (12 bit)
        *Interpreter (512 bytes):               0x0000 - 0x01FF (0 to 511)
            - Fontset (80 bytes):                   0x0000 - 0x004f (0 to 79)
            - Interpreter data (294 bytes):         0x0050 - 0x0175 (80 to 373)
            - Unused region    (138 bytes):         0x0176 - 0x01ff (374 to 511)
        *Program data (3853 bytes):          0x0200 - 0x0FFF (512 to 4095)

    Cpu registers and screen buffer are memory mapped, mapping will be located
    adjacent to the fontset region. A total of 314 bytes need to be allocated,
    256 bytes for the screen buffer and 58 for the memory mapped cpu registers.


    Block size: 80 bytes
    *-------------------* 0x0000
    |      Fontset      |
    +-------------------+ 0x004f

    Block size: 294 bytes
    +-------------------+ 0x0050
    |      Mappings     |
    | Screen (256 bytes)| Screen buffer
    | Stack  ( 16 bytes)| Stack buffer
    | V      ( 16 bytes)| Data registers
    | I      (  2 bytes)| Address register
    | PC     (  2 bytes)| Program counter
    | ST     (  1 byte) | Sound timer
    | DT     (  1 byte) | Delay timer
    |                   |
    +- - - - - - - - - -+ 0x0175

    Block size: 138 bytes
    +- - - - - - - - - -+ 0x0176
    |                   | 
    |     (Unused)      |
    |                   | 
    +-------------------+ 0x01ff

    Block size: 3853 bytes
    +-------------------+ 0x0200 (Chip8 entry point)
    |                   |
    |                   |
    |   PROGRAM ROM     |
    |                   |
    |                   |
    +-------------------+ 0x0FFF

## CPU:
    
    +----------------------+
  _/| STK, SP, PC, I, LF   |\_
  _/| V[0]...V[f]          |\_
  _/| ST, DT               |\_
  _/|         CHIP8        |\_
  _/|                      |\_
  _/|                      |\_
  _/|                      |\_
    +-----------+----------+
                |                   BUS
                +---------------+-----------------------+ 
                                ^  R                    |
                                |                       |
                                |                       |
                                v  W                 W  v
                    +---------------------+     +----------------+
                    |                     |     |                |
                    |       MEMORY        |     |     Display    |
                    |                     |     |                |
                    +---------------------+     +----------------+

    * V[0-F], DT, ST (8 bit): Chip 8 cpu has 16 general purpose 8-bit registers
    (0 to F) that can be read and written directly, and two special purpose
    registers sound timer (ST) and delay timer (DT) of the same capacity,
    the later decrease their set counters at a 60 Hz frequency
    when DS or DT != 0;

    Last register (F) is used as a bit flag by some instructions, programs
    should not rely on it.

    * I (16-bit): It also has a special 16-bit register (I), which is used to
    store memory addresses, so only the lower 12 bits are used.

    * PC (16-bit): Program Counter, holds the currently executing address.

    * SP, Stack[16] (16 bit): Stack pointer, points to the top stack element.
    Stack size must be at least 16 but might be larger to support deeper
    subroutine nesting.

## Keyboard input:
    By default, chip8 keyboard consists on 16 keys, labeled 0 - F. But modern
    implementations allow for remapping to easier keyboard sectors.
    Original hardware looked something like this:
            
            +---+---+---+---+
            | 1 | 2 | 3 | C |
            +---+---+---+---+
            | 4 | 5 | 6 | D |
            +---+---+---+---+
            | 7 | 8 | 9 | E |
            +---+---+---+---+
            | A | 0 | B | F |
            +---+---+---+---+

    Key presses are usually stored at register V[0] by default, but can be put
    and read from any, except V[f].

## Display and graphics:

    Original chip8 display dimensions: 64 x 32

       +--------------------------------+
       |(0,0)                     (63,0)|
       |                                |
       |                                |
       |(0,31)                   (63,31)|
       +--------------------------------+

    each coordinate pair (x,y) corresponds to a pixel that can be either 0 or 1,
    on or off.

    The screen buffer can also be compressed, where each row is stored in a 
    std::uint64_t variable and each pixel is represented by the bits in such
    number; then we just need a 32 element array to store it.
    
    std::array<std::uint64_t, SCREEN_WIDTH> screen_buffer {};

    Internal buffer mapping (X,Y):
       +--------------------------------+
       |(63,0)                     (0,0)|
       |                                |
       |                                |
       |(63,31)                   (0,31)|
       +--------------------------------+

    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 

    Y -> array index 
    X -> ???

    Sprites:

    Sprites are groups of bits, hexadecimal encoded, that indicate whether a
    pixel in the sprite should be turned on or off (0 or 1) when drawn at (x,y)
    screen coordinates. Chip8 interpreter contains a default set of sprites that
    act as fontset that allows to draw characters 0-9 and A-F, each sprite has
    5 bytes of data, and each byte encode one row.
    
    For example, the character 'F':
                0b 1 1 1 1  0 0 0 0 -> 0xF0
                0b 1 0 0 0  0 0 0 0 -> 0x80
                0b 1 1 1 1  0 0 0 0 -> 0xF0
                0b 1 0 0 0  0 0 0 0 -> 0x80
                0b 1 0 0 0  0 0 0 0 -> 0x80

    Therefore, the byte array {0xF0, 0x80, 0xF0, 0x80, 0x80} represents the
    sprite of the character 'F'. All characters have a 4 pixel right-padding as
    its easier to display spaces that way.

    Sprite compression:

    A convenient way to store each font sprite, is to compress the byte arrays
    removing the padding nibble of each row, and joining the active bits of
    the sprite into a single hexadecimal value.

    Thereofre, the byte array for the sprite 'F' {0xF0, 0x80, 0xF0, 0x80, 0x80} 
    will become the 32-bit number 0x000F8F88, but we can ignore the 0's to the
    left, leaving us with 20 bits worth of data, 0xF8F88 effectively halving
    the storage space from 5 bytes per sprite to 2.5 bytes, a 2 to 1 compression
    rate.

    The decompression routine is Big Endian, MSB is decoded first.

    Decompression routine:
    0. Get compressed sprite bytestream.
    1. Initialize 'shift_bits' to 16.
    2. Right shift current bytesream by 'shift_bits' bit-places.
    3. Extract lowest 4 bits of the shift result and left shift 4 bit-places.
    4. Store the result at target memory location.
    5. Substract 4 from 'shift_bits'.
    6. Increment target memory iterator / pointer.
    7. If 'shiftBits' is not 0, go to step 2, else increment compressed_fonts
    iterator / pointer and continue.
    8. If current compressed bytestream is not at the last element, goto step 0,
    else end routine.

    C++ Implementation example:

    static const std::array<uint32_t, NUM_FONTS> compressed_fonts =
    {
        0xF999F, 0x26227, 0xF1F8F, 0xF1F1F, // 0, 1, 2, 3
        0x99F11, 0xF8F1F, 0xF8F9F, 0xF1244, // 4, 5, 6, 7
        0xF9F9F, 0xF9F1F, 0xF9F99, 0xE9E9E, // 8, 9, A, B
        0xF888F, 0xE999E, 0xF8F8F, 0xF8F88  // C, D, E, F
    };

    void decompress_fonts()
    {
        auto *target = chip8::memory->fontset;
        for (const auto& bytestream : compressed_fonts)
        {
            for (int shift_bits = 16; shift_bits >= 0; shift_bits -= 4)
                *target++ = static_cast<uint16_t>(
                    ((bytestream >> shift_bits) & 0x000f) << 4
                );
        }
    }

    NOTE: Both the array and the function may be declared as constexpr for
    compile time evaluation.

    The developer can define custom sprites as long as they don't exceed a total 
    size of 15 bytes per sprite, for a max size of 8x15 (8 columns x 15 rows)

    This fontset must be stored in the interpreter memory area (0x0000 to 0x01FF) 
    and is usually located in the first 80 bytes, 0x0000 to 0x0050.

## Registers
    Chip 8 provides 2 timers, one for delay and another for sound.

    The delay timer (DT) is active whenever its value is >= 0, it substracts 1
    from itself at a rate of 60Hz, when it reaches 0 it deactivates.

    The sound timer (ST) is active whenever its value is >= 0 as well, it also
    decrements itself at a rate of 60Hz, and it will buzz as long as it is
    greater than 0, when it reaches 0 it deactivates.

    The sound Chip 8 emits is only one tone, whose frequency can be defined by
    the author of the interpreter.
    
## Instructions (opcodes)

    The original implementation of Chip 8 included 36 instructions for math,
    display and flow control.

    Super chip 48 included 10 additional, for a total of 46.

    All insctuctions are 2 bytes long and are stored most-significant bit first
    (big-endian). In memory, the fist byte of the instruction must be located
    at an even address, if the program includes sprite data, it should be padded
    so following instructions are properly situated at an even address in RAM.

    This is a list of the variables used:
        * nnn or addr - A 12-bit value representing a direct address in memory,
                        the lowest 12 bits of the instruction.
        * n or nibble - A 4-bit value, lowest 4 bits of the instruction.
        * x and y     - A 4-bit value, x is the lowest 4 bits of the high-byte
                        of the instruction;
                        y is the upper 4 bits of the lower-byte of the
                        instruction.
        * kk or byte  - An 8-bit value, the lower byte of the instruction.

    Bitmasks to extract data to the used variables from instruction bytes:
        nnn - 0x0FFF -> 0000 1111 1111 1111
        n   - 0x000F -> 0000 0000 0000 1111
        x   - 0x0F00 -> 0000 1111 0000 0000
        x   - 0x00F0 -> 0000 0000 1111 0000
        kk  - 0x00FF -> 0000 0000 1111 1111

    we can use any bitwise operations to get, set or toggle said bits using
    AND, OR and XOR respectively.

### Opcode table

```
    +----------------------+
    |       Operands       |
    | nnn   n   x   y   kk |  mnemonic     bytes   description / operations
    +-----+---+---+---+----+--------------+------+-----------------------------+
  0 |  x  |   |   |   |    | SYS nnn      | 0nnn | Jumps to routine at nnn.    |
    |     |   |   |   |    |              |      | Unused by most interpreters |
    |     |   |   |   |    |              |      |                             |
    |     |   |   |   |    | CLR          | 00E0 | Clear screen.               |
    |     |   |   |   |    |              |      |                             |
    |     |   |   |   |    | RET          | 00EE | Return from subr, set pc to |
    |     |   |   |   |    |              |      | poped stack element.        |
    +-----+---+---+---+----+--------------+------+-----------------------------+
  1 |  x  |   |   |   |    | JP nnn       | 1nnn | pc = nnn                    |
    +-----+---+---+---+----+--------------+------+-----------------------------+
  2 |  x  |   |   |   |    | CALL nnn     | 2nnn | Call subr at nnn: push pc to|
    |     |   |   |   |    |              |      | stack and set pc to nnn.    |
    +-----+---+---+---+----+--------------+------+-----------------------------+
  3 |     |   | x |   | x  | SE Vx, kk    | 3xkk | Skip next instruction       |
    |     |   |   |   |    |              |      | if Vx == kk, pc += 2.       |
    +-----+---+---+---+----+--------------+------+-----------------------------+
  4 |     |   | x |   | x  | SNE Vx, kk   | 4xkk | Skip next instrtuction      |
    |     |   |   |   |    |              |      | if Vx != kk, pc += 2.       |
    +-----+---+---+---+----+--------------+------+-----------------------------+
  5 |     |   | x | x |    | SE Vx, Vy    | 5xy0 | Skip next instruction       |
    |     |   |   |   |    |              |      | if Vx != Vy, pc += 2.       |
    +-----+---+---+---+----+--------------+------+-----------------------------+
  6 |     |   | x |   | x  | LD Vx, kk    | 6xkk | Vx = kk                     |
    +-----+---+---+---+----+--------------+------+-----------------------------+
  7 |     |   | x |   | x  | ADD Vx, kk   | 7xkk | Vx = Vx + kk                |
    +-----+---+---+---+----+--------------+------+-----------------------------+
    |     |   | x | x |    | LD  Vx, Vy   | 8xy0 | Vx = Vy                     |
    |     |   |   |   |    |              |      |                             |
    |     |   | x | x |    | OR  Vx, Vy   | 8xy1 | Vx = Vx | Vy                |
    |     |   |   |   |    |              |      |                             |
    |     |   | x | x |    | AND Vx, Vy   | 8xy2 | Vx = Vx & Vy                |
    |     |   |   |   |    |              |      |                             |
    |     |   | x | x |    | XOR Vx, Vy   | 8xy3 | Vx = Vx ^ Vy                |
    |     |   |   |   |    |              |      |                             |
    |     |   | x | x |    | ADD Vx, Vy   | 8xy4 | Vx = Vx - Vy;               |
  8 |     |   |   |   |    |              |      | VF = Vf > 255;              |
    |     |   |   |   |    |              |      | keep low 8-bits of result   |
    |     |   |   |   |    |              |      |                             |
    |     |   | x | x |    | SUB Vx, Vy   | 8xy5 | VF = Vx > Vy;               |
    |     |   |   |   |    |              |      | Vx = Vx - Vy;               |
    |     |   |   |   |    |              |      |                             |
    |     |   | x | x |    | SHR Vx{, Vy} | 8xy6 | VF = Vx & 0x01;             |
    |     |   |   |   |    |              |      | Vx = Vx / 2;                |
    |     |   |   |   |    |              |      |                             |
    |     |   | x | x |    | SUBN Vx, Vy  | 8xy7 | VF = Vy > Vx;               |
    |     |   |   |   |    |              |      | Vx = Vy - Vx;               |
    |     |   |   |   |    |              |      |                             |
    |     |   | x | x |    | SHL Vx{, Vy} | 8xyE | VF = Vx & 0x80;             |
    |     |   |   |   |    |              |      | Vx = Vx * 2;                |
    +-----+---+---+---+----+--------------+------+-----------------------------+
  9 |     |   | x | x |    | SNE Vx, Vy   | 9xy0 | Skip next instruction if    |
    |     |   |   |   |    |              |      | Vx != Vy.                   |
    +-----+---+---+---+----+--------------+------+-----------------------------+
  A |  x  |   |   |   |    | LD I, nnn    | Annn | I = nnn                     |
    +-----+---+---+---+----+--------------+------+-----------------------------+
  B |  x  |   |   |   |    | JP V0, nnn   | Bnnn | pc = nnn + V0               |
    +-----+---+---+---+----+--------------+------+-----------------------------+
  C |     |   |   |   | x  | RND Vx, kk   | Cxkk | Vx = RANDOM_BYTE & kk       |
    +-----+---+---+---+----+--------------+------+-----------------------------+
  D |     | x | x | x |    | DRW Vx, Vy, n| Dxyn | Draw n-byte sprite at addr I|
    |     |   |   |   |    |              |      | on screen coord (Vx, Vy);   |
    |     |   |   |   |    |              |      | VF = Collision;             |
    +-----+---+---+---+----+--------------+------+-----------------------------+
  E |     |   | x |   |    | SKP Vx       | Ex9E | Skip next instruction if    |
    |     |   |   |   |    |              |      | keypad key with value Vx    |
    |     |   |   |   |    |              |      | is pressed.                 |
    |     |   |   |   |    |              |      |                             |
    |     |   | x |   |    | SKNP Vx      | ExA1 | Skip next instruction if    |
    |     |   |   |   |    |              |      | keypad key with value Vx    |
    |     |   |   |   |    |              |      | is not pressed.             |
    +-----+---+---+---+----+--------------+------+-----------------------------+
  F |     |   | x |   |    | LD Vx, DT    | Fx07 | Vx = DT                     |
    |     |   |   |   |    |              |      |                             |
    |     |   | x |   |    | LD Vx, K     | Fx0A | Wait for keypress, then load|
    |     |   |   |   |    |              |      | the key value into Vx.      |
    |     |   |   |   |    |              |      |                             |
    |     |   | x |   |    | LD DT, Vx    | Fx15 | DT = Vx                     |
    |     |   |   |   |    |              |      |                             |
    |     |   | x |   |    | LD ST, Vx    | Fx18 | ST = Vx                     |
    |     |   |   |   |    |              |      |                             |
    |     |   | x |   |    | ADD I, Vx    | Fx1E | I = I + Vx                  |
    |     |   |   |   |    |              |      |                             |
    |     |   | x |   |    | LD F, Vx     | Fx29 | Set I to location of sprite |
    |     |   |   |   |    |              |      | for digit Vx in fontset.    |
    |     |   |   |   |    |              |      |                             |
    |     |   | x |   |    | LD B, Vx     | Fx33 | Store BCD representation of |
    |     |   |   |   |    |              |      | Vx in I, I+1 and I+2 (HTU)  |
    |     |   |   |   |    |              |      | Hundreds, Tenths, Units.    |
    |     |   |   |   |    |              |      |                             |
    |     |   | x |   |    | LD [I], Vx   | Fx55 | Store registers V0 ... Vx   |
    |     |   |   |   |    |              |      | in location I, I+1... I + Vx|
    |     |   |   |   |    |              |      |                             |
    |     |   | x |   |    | LD Vx, [I]   | Fx65 | Read values of I ... I + Vx |
    |     |   |   |   |    |              |      | and store them on registers |
    |     |   |   |   |    |              |      | V0 to Vx                    |
    +-----+---+---+---+----+--------------+------+-----------------------------+
```