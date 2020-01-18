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
namespace opcode {

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

/// @brief Operands register Vx.
struct OperandsX
{
    unsigned x : 4;
};

/// @brief Operands register Vx,byte.
struct OperandsXKK
{
    unsigned x  : 4;
    unsigned kk : 8;
};

/// @brief Operands register Vx,Vy.
struct OperandsXY
{
    unsigned x : 4;
    unsigned y : 4;
};

// @brief Operands Vx,Vy,nibble
struct OperandsXYN
{
    unsigned x : 4;
    unsigned y : 4;
    unsigned n : 4;
};

/// @brief Operands NNN
struct OperandsNNN
{
    unsigned nnn : 12;
};

/// @brief Encode opcode 00E0
///
/// @return Opcode.
inline Opcode encode00E0()
{
    return OPCODE_00E0;
}

/// @brief Decode opcode 00E0
///
/// @param opcode Opcode.
/// @return Opcode.
inline Opcode decode00E0(Opcode opcode)
{
    return opcode & OPCODE_00E0;
}

/// @brief Encode opcode 00EE
///
/// @return Opcode.
inline Opcode encode00EE()
{
    return OPCODE_00EE;
}

/// @brief Decode opcode 00E0
///
/// @param opcode Opcode.
/// @return Opcode.
inline Opcode decode00EE(Opcode opcode)
{
    return opcode & OPCODE_00E0;
}

/// @brief Encode opcode 0NNN
///
/// @param nnn Address.
/// @return Operands.
inline Opcode encode0NNN(uint16_t nnn)
{
    OperandsNNN operands;

    operands.nnn = nnn;

    return OPCODE_0NNN | operands.nnn;
}

/// @brief Decode opcode 0NNN
///
/// @param operands Operands.
/// @return Operands.
inline OperandsNNN decode0NNN(Opcode opcode)
{
    OperandsNNN operands;

    operands.nnn = opcode & 0xFFF;

    return operands;
}

/// @brief Encode opcode 1NNN
///
/// @param nnn Address.
/// @return Operands.
inline Opcode encode1NNN(uint16_t nnn)
{
    OperandsNNN operands;

    operands.nnn = nnn;

    return OPCODE_1NNN | operands.nnn;
}

/// @brief Decode opcode 1NNN
///
/// @param operands Operands.
/// @return Operands.
inline OperandsNNN decode1NNN(Opcode opcode)
{
    OperandsNNN operands;

    operands.nnn = opcode & 0xFFF;

    return operands;
}

/// @brief Encode opcode 2NNN
///
/// @param nnn Address.
/// @return Operands.
inline Opcode encode2NNN(uint16_t nnn)
{
    OperandsNNN operands;

    operands.nnn = nnn;

    return OPCODE_2NNN | operands.nnn;
}

/// @brief Decode opcode 2NNN
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsNNN decode2NNN(Opcode opcode)
{
    OperandsNNN operands;

    operands.nnn = opcode & 0xFFF;

    return operands;
}

/// @brief Encode opcode 3XKK
///
/// @param x  Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode encode3XKK(uint16_t x, uint16_t kk)
{
    OperandsXKK operands;

    operands.x = x;
    operands.kk = kk;

    return OPCODE_3XKK | (operands.x << 8) | operands.kk;
}

/// @brief Decode opcode 3XKK
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsXKK decode3XKK(Opcode opcode)
{
    OperandsXKK operands;

    operands.x = (opcode >> 8) & 0xF;
    operands.kk = opcode & 0xFF;

    return operands;
};

/// @brief Encode opcode 4XKK
///
/// @param x  Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode encode4XKK(uint16_t x, uint16_t kk)
{
    OperandsXKK operands;

    operands.x = x;
    operands.kk = kk;

    return OPCODE_4XKK | (operands.x << 8) | operands.kk;
}

/// @brief Decode opcode 4XKK
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsXKK decode4XKK(Opcode opcode)
{
    OperandsXKK operands;

    operands.x = (opcode >> 8) & 0xF;
    operands.kk = opcode & 0xFF;

    return operands;
};

/// @brief Encode opcode 5XY0
///
/// @param x  Vx.
/// @param y  Vy.
/// @return Opcode.
inline Opcode encode5XY0(uint16_t x, uint16_t y)
{
    OperandsXY operands;

    operands.x = x;
    operands.y = y;

    return OPCODE_5XY0 | (operands.x << 8) | (operands.y << 4);
}

/// @brief Decode opcode 5XKK
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsXY decode5YX0(Opcode opcode)
{
    OperandsXY operands;

    operands.x = (opcode >> 8) & 0xF;
    operands.y = (opcode >> 4) & 0xF;

    return operands;
};

/// @brief Encode opcode 6XKK
///
/// @param x  Register Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode encode6XKK(uint16_t x, uint16_t kk)
{
    OperandsXKK operands;

    operands.x = x;
    operands.kk = kk;

    return OPCODE_6XKK | (operands.x << 8) | operands.kk;
}

/// @brief Decode opcode 6XKK
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsXY decode6YX0(Opcode opcode)
{
    OperandsXY operands;

    operands.x = (opcode >> 8) & 0xF;
    operands.y = (opcode >> 4) & 0xF;

    return operands;
}

/// @brief Encode opcode 7XKK
///
/// @param x  Register Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode encode7XKK(uint16_t x, uint16_t kk)
{
    OperandsXKK operands;

    operands.x = x;
    operands.kk = kk;

    return OPCODE_7XKK | (operands.x << 8) | operands.kk;
}

/// @brief Decode opcode 7XKK
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsXKK decode7XKK(Opcode opcode)
{
    OperandsXKK operands;

    operands.x = (opcode >> 8) & 0xF;
    operands.kk = (opcode >> 4) & 0xF;

    return operands;
}

/// @brief Encode opcode 8XY0
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY0(uint16_t x, uint16_t y)
{
    OperandsXY operands;

    operands.x = x;
    operands.y = y;

    return OPCODE_8XY0 | (operands.x << 8) | (operands.y << 4);
}

/// @brief Decode opcode 8XY0
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsXY decode8XY0(Opcode opcode)
{
    OperandsXY operands;

    operands.x = (opcode >> 8) & 0xF;
    operands.y = (opcode >> 4) & 0xF;

    return operands;
}

/// @brief Encode opcode 8XY1
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY1(uint16_t x, uint16_t y)
{
    OperandsXY operands;

    operands.x = x;
    operands.y = y;

    return OPCODE_8XY1 | (operands.x << 8) | (operands.y << 4);
}

/// @brief Decode opcode 8XY1
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsXY decode8XY1(Opcode opcode)
{
    OperandsXY operands;

    operands.x = (opcode >> 8) & 0xF;
    operands.y = (opcode >> 4) & 0xF;

    return operands;
}

/// @brief Encode opcode 8XY2
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY2(uint16_t x, uint16_t y)
{
    OperandsXY operands;

    operands.x = x;
    operands.y = y;

    return OPCODE_8XY2 | (operands.x << 8) | (operands.y << 4);
}

/// @brief Decode opcode 8XY2
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsXY decode8XY2(Opcode opcode)
{
    OperandsXY operands;

    operands.x = (opcode >> 8) & 0xF;
    operands.y = (opcode >> 4) & 0xF;

    return operands;
}

/// @brief Encode opcode 8XY3
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY3(uint16_t x, uint16_t y)
{
    OperandsXY operands;

    operands.x = x;
    operands.y = y;

    return OPCODE_8XY3 | (operands.x << 8) | (operands.y << 4);
}

/// @brief Decode opcode 8XY3
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsXY decode8XY3(Opcode opcode)
{
    OperandsXY operands;

    operands.x = (opcode >> 8) & 0xF;
    operands.y = (opcode >> 4) & 0xF;

    return operands;
}

/// @brief Encode opcode 8XY4
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY4(uint16_t x, uint16_t y)
{
    OperandsXY operands;

    operands.x = x;
    operands.y = y;

    return OPCODE_8XY4 | (operands.x << 8) | (operands.y << 4);
}

/// @brief Decode opcode 8XY4
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsXY decode8XY4(Opcode opcode)
{
    OperandsXY operands;

    operands.x = (opcode >> 8) & 0xF;
    operands.y = (opcode >> 4) & 0xF;

    return operands;
}

/// @brief Encode opcode 8XY5
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY5(uint16_t x, uint16_t y)
{
    OperandsXY operands;

    operands.x = x;
    operands.y = y;

    return OPCODE_8XY5 | (operands.x << 8) | (operands.y << 4);
}

/// @brief Decode opcode 8XY5
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsXY decode8XY5(Opcode opcode)
{
    OperandsXY operands;

    operands.x = (opcode >> 8) & 0xF;
    operands.y = (opcode >> 4) & 0xF;

    return operands;
}

/// @brief Encode opcode 8XY6
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY6(uint16_t x, uint16_t y)
{
    OperandsXY operands;

    operands.x = x;
    operands.y = y;

    return OPCODE_8XY6 | (operands.x << 8) | (operands.y << 4);
}

/// @brief Decode opcode 8XY6
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsXY decode8XY6(Opcode opcode)
{
    OperandsXY operands;

    operands.x = (opcode >> 8) & 0xF;
    operands.y = (opcode >> 4) & 0xF;

    return operands;
}

/// @brief Encode opcode 8XY7
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY7(uint16_t x, uint16_t y)
{
    OperandsXY operands;

    operands.x = x;
    operands.y = y;

    return OPCODE_8XY7 | (operands.x << 8) | (operands.y << 4);
}

/// @brief Decode opcode 8XY7
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsXY decode8XY7(Opcode opcode)
{
    OperandsXY operands;

    operands.x = (opcode >> 8) & 0xF;
    operands.y = (opcode >> 4) & 0xF;

    return operands;
}

/// @brief Encode opcode 9XY0
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode9XY0(uint16_t x, uint16_t y)
{
    OperandsXY operands;

    operands.x = x;
    operands.y = y;

    return OPCODE_9XY0 | (operands.x << 8) | (operands.y << 4);
}

/// @brief Decode opcode 9XY0
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsXY decode9XY0(Opcode opcode)
{
    OperandsXY operands;

    operands.x = (opcode >> 8) & 0xF;
    operands.y = (opcode >> 4) & 0xF;

    return operands;
}

/// @brief Encode opcode ANNN
///
/// @param nnn Address.
/// @return Opcode.
inline Opcode encodeANNN(uint16_t nnn)
{
    OperandsNNN operands;

    operands.nnn = nnn;

    return OPCODE_ANNN | operands.nnn;
}

/// @brief Decode opcode ANNN
///
/// @param operands Operands.
/// @return Operands.
inline OperandsNNN decodeANNN(Opcode opcode)
{
    OperandsNNN operands;

    operands.nnn = opcode & 0xFFF;

    return operands;
}

/// @brief Encode opcode BNNN
///
/// @param nnn Address.
/// @return Opcode.
inline Opcode encodeBNNN(uint16_t nnn)
{
    OperandsNNN operands;

    operands.nnn = nnn;

    return OPCODE_BNNN | operands.nnn;
}


/// @brief Decode opcode BNNN
///
/// @param operands Operands.
/// @return Operands.
inline OperandsNNN decodeBNNN(Opcode opcode)
{
    OperandsNNN operands;

    operands.nnn = opcode & 0xFFF;

    return operands;
}

/// @brief Encode opcode CXKK
///
/// @param x  Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode encodeCXKK(uint16_t x, uint16_t kk)
{
    OperandsXKK operands;

    operands.x = x;
    operands.kk = kk;

    return OPCODE_CXKK | (operands.x << 8) | operands.kk;
}

/// @brief Decode opcode CXKK
///
/// @param opcode Opcode.
/// @return Operands.
inline OperandsXKK decodeCXKK(Opcode opcode)
{
    OperandsXKK operands;

    operands.x = (opcode >> 8) & 0xF;
    operands.kk = (opcode >> 4) & 0xF;

    return operands;
}


/// @brief Encode opcode DXYN
///
/// @param x  Vx.
/// @param y  Vy.
/// @param n  Address.
/// @return Opcode.
inline Opcode encodeDXYN(uint16_t x, uint16_t y, uint16_t n)
{
    OperandsXYN operands;

    operands.x = x;
    operands.y = y;
    operands.n = n;

    return OPCODE_DXYN | (operands.x << 8) | (operands.y << 4) | operands.n;
}

/// @brief Encode opcode EX9E
///
/// @param x  Vx.
/// @return Opcode.
inline Opcode encodeEX9E(uint16_t x)
{
    OperandsX operands;

    operands.x = x;

    return OPCODE_EX9E | (operands.x << 8);
}

/// @brief Encode opcode EXA1
///
/// @param x  Vx.
/// @return Opcode.
inline Opcode encodeEXA1(uint16_t x)
{
    OperandsX operands;

    operands.x = x;

    return OPCODE_EXA1 | (operands.x << 8);
}

/// @brief Encode opcode FX07
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX07(uint16_t x)
{
    OperandsX operands;

    operands.x = x;

    return OPCODE_FX07 | (operands.x << 8);
}

/// @brief Encode opcode FX0A
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX0A(uint16_t x)
{
    OperandsX operands;

    operands.x = x;

    return OPCODE_FX0A | (operands.x << 8);
}

/// @brief Encode opcode FX15
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX15(uint16_t x)
{
    OperandsX operands;

    operands.x = x;

    return OPCODE_FX15 | (operands.x << 8);
}

/// @brief Encode opcode FX18
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX18(uint16_t x)
{
    OperandsX operands;

    operands.x = x;

    return OPCODE_FX18 | (operands.x << 8);
}

/// @brief Encode opcode FX1E
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX1E(uint16_t x)
{
    OperandsX operands;

    operands.x = x;

    return OPCODE_FX1E | (operands.x << 8);
}

/// @brief Encode opcode FX29
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX29(uint16_t x)
{
    OperandsX operands;

    operands.x = x;

    return OPCODE_FX29 | (operands.x << 8);
}

/// @brief Encode opcode FX33
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX33(uint16_t x)
{
    OperandsX operands;

    operands.x = x;

    return OPCODE_FX33 | (operands.x << 8);
}

/// @brief Encode opcode FX55
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX55(uint16_t x)
{
    OperandsX operands;

    operands.x = x;

    return OPCODE_FX55 | (operands.x << 8);
}

/// @brief Encode opcode FX65
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX65(uint16_t x)
{
    OperandsX operands;

    operands.x = x;

    return OPCODE_FX65 | (operands.x << 8);
}

}  // opcode
}  // chip8

#endif  // CHIP8_OPCODE_HPP
