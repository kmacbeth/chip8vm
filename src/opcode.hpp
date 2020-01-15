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
enum OpcodeDecoding
{
    OPCODE_6XKK = 0x6000,
    OPCODE_8XY0 = 0x8000,
    OPCODE_ANNN = 0xA000,
    OPCODE_FX07 = 0xF007,
    OPCODE_FX15 = 0xF015,
    OPCODE_FX18 = 0xF018,
};

/// @brief Build opcode 6XKK
///
/// @param x  Register Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode opcode6XKK(uint16_t x, uint16_t kk)
{
    return OPCODE_6XKK | ((x & 0xF) << 8) | (kk & 0xFF);
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

/// @brief Build opcode ANNN
///
/// @param nnn Address.
/// @return Opcode.
inline Opcode opcodeANNN(uint16_t nnn)
{
    return OPCODE_ANNN | (nnn & 0xFFF);
}
/// @brief Build opcode FX07
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode opcodeFX07(uint16_t x)
{
    return OPCODE_FX07 | ((x & 0xF) << 8);
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

}  // chip8

#endif  // CHIP8_OPCODE_HPP
