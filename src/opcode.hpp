/*
 * MIT License
 *
 * Copyright (c) 2020 Martin Lafreniere
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef CHIP8_OPCODE_HPP
#define CHIP8_OPCODE_HPP

#include <core.hpp>

namespace chip8 {

using Opcode = uint16_t;

/// @brief Opcodes definitions
enum OpcodeEncoding
{
    OPCODE_00E0 = 0x00E0,
    OPCODE_00EE = 0x00EE,
    OPCODE_0NNN = 0x0000,
    OPCODE_1NNN = 0x1000,
    OPCODE_2NNN = 0x2000,
    OPCODE_3XKK = 0x3000,
    OPCODE_4XKK = 0x4000,
    OPCODE_5XY0 = 0x5000,
    OPCODE_6XKK = 0x6000,
    OPCODE_7XKK = 0x7000,
    OPCODE_8XY0 = 0x8000,
    OPCODE_8XY1 = 0x8001,
    OPCODE_8XY2 = 0x8002,
    OPCODE_8XY3 = 0x8003,
    OPCODE_8XY4 = 0x8004,
    OPCODE_8XY5 = 0x8005,
    OPCODE_8XY6 = 0x8006,
    OPCODE_8XY7 = 0x8007,
    OPCODE_8XYE = 0x800E,
    OPCODE_9XY0 = 0x9000,
    OPCODE_ANNN = 0xA000,
    OPCODE_BNNN = 0xB000,
    OPCODE_CXKK = 0xC000,
    OPCODE_DXYN = 0xD000,
    OPCODE_EX9E = 0xE09E,
    OPCODE_EXA1 = 0xE0A1,
    OPCODE_FX07 = 0xF007,
    OPCODE_FX0A = 0xF00A,
    OPCODE_FX15 = 0xF015,
    OPCODE_FX18 = 0xF018,
    OPCODE_FX1E = 0xF01E,
    OPCODE_FX29 = 0xF029,
    OPCODE_FX33 = 0xF033,
    OPCODE_FX55 = 0xF055,
    OPCODE_FX65 = 0xF065
};

/// @brief Build opcode 00E0
///
/// @return Opcode.
inline Opcode opcode00E0()
{
    return OPCODE_00E0;
}

/// @brief Build opcode 00EE
///
/// @return Opcode.
inline Opcode opcode00EE()
{
    return OPCODE_00EE;
}

/// @brief Build opcode 0NNN
///
/// @param nnn Address.
/// @return Opcode.
inline Opcode opcode0NNN(uint16_t nnn)
{
    return OPCODE_0NNN | (nnn & 0xFFF);
}

/// @brief Build opcode 1NNN
///
/// @param nnn Address.
/// @return Opcode.
inline Opcode opcode1NNN(uint16_t nnn)
{
    return OPCODE_1NNN | (nnn & 0xFFF);
}

/// @brief Build opcode 2NNN
///
/// @param nnn Address.
/// @return Opcode.
inline Opcode opcode2NNN(uint16_t nnn)
{
    return OPCODE_2NNN | (nnn & 0xFFF);
}

/// @brief Build opcode 3XKK
///
/// @param x  Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode opcode3XKK(uint16_t x, uint16_t kk)
{
    return OPCODE_3XKK | ((x & 0xF) << 8) | (kk & 0xFF);
}

/// @brief Build opcode 4XKK
///
/// @param x  Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode opcode4XKK(uint16_t x, uint16_t kk)
{
    return OPCODE_4XKK | ((x & 0xF) << 8) | (kk & 0xFF);
}

/// @brief Build opcode 5XY0
///
/// @param x  Vx.
/// @param y  Vy.
/// @return Opcode.
inline Opcode opcode5XKK(uint16_t x, uint16_t y)
{
    return OPCODE_5XY0 | ((x & 0xF) << 8) | ((y & 0xF) << 4);
}

/// @brief Build opcode 6XKK
///
/// @param x  Register Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode opcode6XKK(uint16_t x, uint16_t kk)
{
    return OPCODE_6XKK | ((x & 0xF) << 8) | (kk & 0xFF);
}

/// @brief Build opcode 7XKK
///
/// @param x  Register Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode opcode7XKK(uint16_t x, uint16_t kk)
{
    return OPCODE_7XKK | ((x & 0xF) << 8) | (kk & 0xFF);
}

/// @brief Build opcode 8XY0
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode opcode8XY0(uint16_t x, uint16_t y)
{
    return OPCODE_8XY0 | ((x & 0xF) << 8) | ((y & 0xF) << 4);
}

/// @brief Build opcode 8XY1
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode opcode8XY1(uint16_t x, uint16_t y)
{
    return OPCODE_8XY1 | ((x & 0xF) << 8) | ((y & 0xF) << 4);
}

/// @brief Build opcode 8XY2
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode opcode8XY2(uint16_t x, uint16_t y)
{
    return OPCODE_8XY2 | ((x & 0xF) << 8) | ((y & 0xF) << 4);
}

/// @brief Build opcode 8XY3
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode opcode8XY3(uint16_t x, uint16_t y)
{
    return OPCODE_8XY3 | ((x & 0xF) << 8) | ((y & 0xF) << 4);
}

/// @brief Build opcode 8XY4
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode opcode8XY4(uint16_t x, uint16_t y)
{
    return OPCODE_8XY4 | ((x & 0xF) << 8) | ((y & 0xF) << 4);
}

/// @brief Build opcode 8XY5
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode opcode8XY5(uint16_t x, uint16_t y)
{
    return OPCODE_8XY5 | ((x & 0xF) << 8) | ((y & 0xF) << 4);
}

/// @brief Build opcode 8XY6
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode opcode8XY6(uint16_t x, uint16_t y)
{
    return OPCODE_8XY6 | ((x & 0xF) << 8) | ((y & 0xF) << 4);
}

/// @brief Build opcode 8XY7
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode opcode8XY7(uint16_t x, uint16_t y)
{
    return OPCODE_8XY7 | ((x & 0xF) << 8) | ((y & 0xF) << 4);
}

/// @brief Build opcode 9XY0
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode opcode9XY0(uint16_t x, uint16_t y)
{
    return OPCODE_9XY0 | ((x & 0xF) << 8) | ((y & 0xF) << 4);
}

/// @brief Build opcode ANNN
///
/// @param nnn Address.
/// @return Opcode.
inline Opcode opcodeANNN(uint16_t nnn)
{
    return OPCODE_ANNN | (nnn & 0xFFF);
}

/// @brief Build opcode BNNN
///
/// @param nnn Address.
/// @return Opcode.
inline Opcode opcodeBNNN(uint16_t nnn)
{
    return OPCODE_BNNN | (nnn & 0xFFF);
}

/// @brief Build opcode CXKK
///
/// @param x  Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode opcodeCXKK(uint16_t x, uint16_t kk)
{
    return OPCODE_CXKK | ((x & 0xF) << 8) | (kk & 0xFF);
}

/// @brief Build opcode DXYN
///
/// @param x  Vx.
/// @param y  Vy.
/// @param n  Address.
/// @return Opcode.
inline Opcode opcodeDXYN(uint16_t x, uint16_t y, uint16_t n)
{
    return OPCODE_DXYN | ((x & 0xF) << 8) | ((y & 0xF) << 4) | (n & 0xF);
}

/// @brief Build opcode EX9E
///
/// @param x  Vx.
/// @return Opcode.
inline Opcode opcodeEX9E(uint16_t x)
{
    return OPCODE_EX9E | ((x & 0xF) << 8);
}

/// @brief Build opcode EXA1
///
/// @param x  Vx.
/// @return Opcode.
inline Opcode opcodeEXA1(uint16_t x)
{
    return OPCODE_EXA1 | ((x & 0xF) << 8);
}

/// @brief Build opcode FX07
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode opcodeFX07(uint16_t x)
{
    return OPCODE_FX07 | ((x & 0xF) << 8);
}

/// @brief Build opcode FX0A
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode opcodeFX0A(uint16_t x)
{
    return OPCODE_FX0A | ((x & 0xF) << 8);
}

/// @brief Build opcode FX15
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode opcodeFX15(uint16_t x)
{
    return OPCODE_FX15 | ((x & 0xF) << 8);
}

/// @brief Build opcode FX18
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode opcodeFX18(uint16_t x)
{
    return OPCODE_FX18 | ((x & 0xF) << 8);
}

/// @brief Build opcode FX1E
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode opcodeFX1E(uint16_t x)
{
    return OPCODE_FX1E | ((x & 0xF) << 8);
}

/// @brief Build opcode FX29
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode opcodeFX29(uint16_t x)
{
    return OPCODE_FX29 | ((x & 0xF) << 8);
}

/// @brief Build opcode FX33
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode opcodeFX33(uint16_t x)
{
    return OPCODE_FX33 | ((x & 0xF) << 8);
}

/// @brief Build opcode FX55
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode opcodeFX55(uint16_t x)
{
    return OPCODE_FX55 | ((x & 0xF) << 8);
}

/// @brief Build opcode FX65
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode opcodeFX65(uint16_t x)
{
    return OPCODE_FX65 | ((x & 0xF) << 8);
}

}  // chip8

#endif  // CHIP8_OPCODE_HPP
