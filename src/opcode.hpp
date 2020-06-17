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

#include <cstdio>
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

/// @brief Decode instruction from opcode.
///
/// @param opcode  Opcode.
/// @return Decoded instruction.
inline Opcode decodeInstruction(Opcode opcode)
{
    uint16_t decodedOpcode = opcode & 0xF000;

    if (decodedOpcode == 0x0000 || decodedOpcode == 0xE000 || decodedOpcode == 0xF000)
    {
        decodedOpcode |= (opcode & 0x00FF);
    }
    else if (decodedOpcode == 0x5000 | decodedOpcode == 0x8000 || decodedOpcode == 0x9000)
    {
        decodedOpcode |= (opcode & 0x000F);
    }

    return decodedOpcode;
}

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


/// @brief Operands utilities.
struct Operand
{
    /// @brief Vx index.
    struct X
    {
        X(uint16_t x) : x(x) {}

        uint16_t x;
    };

    /// @brief Vx index plus byte.
    struct XKK : X
    {
        XKK(uint16_t x, uint16_t kk) : X(x), kk(kk) {}

        uint16_t kk;
    };

    /// @brief Vx plus Vy.
    struct XY : X
    {
        XY(uint16_t x, uint16_t y) : X(x), y(y) {}

        uint16_t y;
    };

    /// @brief Vx plus Vy plus nibble.
    struct XYN : XY
    {
        XYN(uint16_t x, uint16_t y, uint16_t n) : XY(x, y), n(n) {}

        uint16_t n;
    };

    /// @brief Address.
    struct NNN
    {
        NNN(uint16_t nnn) : nnn(nnn) {}

        uint16_t nnn;
    };

    template<typename OP>
    static Opcode join(OP const& op);

    template<typename OP>
    static OP split(Opcode opcode);

};

/// @brief Join Vx.
///
/// @param x  Vx register.
/// @return Joined operand.
template<>
inline Opcode Operand::join(X const& op)
{
    return ((op.x & 0xF) << 8);
}

/// @brief Join Vx and byte.
///
/// @param x  Vx register.
/// @param kk Byte value.
/// @return Joined operands.
template<>
inline Opcode Operand::join(XKK const& op)
{
    return join<X>(op) | (op.kk & 0xFF);
}

/// @brief Join Vx and Vy
///
/// @param x  Vx register.
/// @param y  Vy register.
/// @return Joined operands.
template<>
inline Opcode Operand::join(XY const& op)
{
    return join<X>(op) | ((op.y & 0xF) << 4);
}

/// @brief Join Vx, Vy and nibble
///
/// @param x  Vx register.
/// @param y  Vy register.
/// @param n  Nibble.
/// @return Joined operands.
template<>
inline Opcode Operand::join(XYN const& op)
{
    return join<XY>(op) | (op.n & 0xF);
}

/// @brief Join address.
///
/// @param nnn Address.
/// @return Joined operand.
template<>
inline Opcode Operand::join(NNN const& op)
{
    return (op.nnn & 0xFFF);
}

/// @brief Split Vx.
///
/// @param opcode Opcode.
/// @return X object.
template<>
inline Operand::X Operand::split(Opcode opcode)
{
    return Operand::X((opcode >> 8) & 0xF);
}

/// @brief Split Vx and kk.
///
/// @param opcode Opcode.
/// @return X object.
template<>
inline Operand::XKK Operand::split(Opcode opcode)
{
    return Operand::XKK((opcode >> 8) &0xF, opcode & 0xFF);
}

/// @brief Split Vx and Vy.
///
/// @param opcode Opcode.
/// @return X object.
template<>
inline Operand::XY Operand::split(Opcode opcode)
{
    return Operand::XY((opcode >> 8) & 0xF, (opcode >> 4) & 0xF);
}

/// @brief Split Vx and Vy and nibble.
///
/// @param opcode Opcode.
/// @return X object.
template<>
inline Operand::XYN Operand::split(Opcode opcode)
{
    return Operand::XYN((opcode >> 8) & 0xF, (opcode >> 4) & 0xF, opcode & 0xF);
}

/// @brief Split address.
///
/// @param opcode Opcode.
/// @return NNN object.
template<>
inline Operand::NNN Operand::split(Opcode opcode)
{
    return Operand::NNN(opcode & 0xFFF);
}

/// @brief Trace opcode.
///
/// @param opcode Opcode.
inline void traceOpcode(Opcode opcode)
{
    std::printf("Opcode: 0x%04X | Instruction: 0x%04X", opcode, decodeInstruction(opcode));
}

/// @brief Trace separator.
inline void traceSeparator()
{
    std::printf(" | ");
}

/// @brief Trace operand X.
///
/// @param op X operand.
inline void traceOperandX(Operand::X const& op)
{
    std::printf("X = %01X", op.x);
}

/// @brief Trace operand XKK.
///
/// @param op XKK operand.
inline void traceOperandXKK(Operand::XKK const& op)
{
    traceOperandX(op);
    traceSeparator();

    std::printf("KK = %u", op.kk);
}

/// @brief Trace operand XY.
///
/// @param op XY operand.
inline void traceOperandXY(Operand::XY const& op)
{
    traceOperandX(op);
    traceSeparator();

    std::printf("Y = %01X", op.y);
}

/// @brief Trace operand XYN.
///
/// @param op XYN operand.
inline void traceOperandXYN(Operand::XYN const& op)
{
    traceOperandXY(op);
    traceSeparator();

    std::printf("N = %u", op.n);
}

/// @brief Trace operand NNN.
///
/// @param op NNN operand.
inline void traceOperandNNN(Operand::NNN const& op)
{
    std::printf("NNN = 0x%03X", op.nnn);
}

/// @brief Trace opcode.
///
/// @param opcode Opcode.
inline void trace(Opcode opcode)
{
    traceOpcode(opcode);
    std::puts("");
}

/// @brief Trace operand.
///
/// @param op XYN operand.
template<typename OP>
void trace(Opcode opcode, OP const& op);

/// @brief Trace opcode with operand X.
///
/// @param opcode Opcode.
/// @param op X operand.
template<>
inline void trace(Opcode opcode, Operand::X const& op)
{
    traceOpcode(opcode);
    traceSeparator();
    traceOperandX(op);
    std::puts("");
}

/// @brief Trace opcode with operand XKK.
///
/// @param opcode Opcode.
/// @param op XKK operand.
template<>
inline void trace(Opcode opcode, Operand::XKK const& op)
{
    traceOpcode(opcode);
    traceSeparator();
    traceOperandXKK(op);
    std::puts("");
}

/// @brief Trace opcode with operand XY.
///
/// @param opcode Opcode.
/// @param op XY operand.
template<>
inline void trace(Opcode opcode, Operand::XY const& op)
{
    traceOpcode(opcode);
    traceSeparator();
    traceOperandXY(op);
    std::puts("");
}

/// @brief Trace opcode with operand XYN.
///
/// @param opcode Opcode.
/// @param op XYN operand.
template<>
inline void trace(Opcode opcode, Operand::XYN const& op)
{
    traceOpcode(opcode);
    traceSeparator();
    traceOperandXYN(op);
    std::puts("");
}

/// @brief Trace opcode with operand NNN.
///
/// @param opcode Opcode.
/// @param op NNN operand.
template<>
inline void trace(Opcode opcode, Operand::NNN const& op)
{
    traceOpcode(opcode);
    traceSeparator();
    traceOperandNNN(op);
    std::puts("");
}

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
    return OPCODE_0NNN | Operand::join(Operand::NNN(nnn));
}

/// @brief Decode opcode 0NNN
///
/// @param operands Operands.
/// @return Operands.
inline Operand::NNN decode0NNN(Opcode opcode)
{
    return Operand::split<Operand::NNN>(opcode);
}

/// @brief Encode opcode 1NNN
///
/// @param nnn Address.
/// @return Operands.
inline Opcode encode1NNN(uint16_t nnn)
{
    return OPCODE_1NNN | Operand::join(Operand::NNN(nnn));
}

/// @brief Decode opcode 1NNN
///
/// @param operands Operands.
/// @return Operands.
inline Operand::NNN decode1NNN(Opcode opcode)
{
    return Operand::split<Operand::NNN>(opcode);
}

/// @brief Encode opcode 2NNN
///
/// @param nnn Address.
/// @return Operands.
inline Opcode encode2NNN(uint16_t nnn)
{
    return OPCODE_2NNN | Operand::join(Operand::NNN(nnn));
}

/// @brief Decode opcode 2NNN
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::NNN decode2NNN(Opcode opcode)
{
    return Operand::split<Operand::NNN>(opcode);
}
/// @brief Encode opcode 3XKK
///
/// @param x  Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode encode3XKK(uint16_t x, uint16_t kk)
{
    return OPCODE_3XKK | Operand::join(Operand::XKK(x, kk));
}

/// @brief Decode opcode 3XKK
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XKK decode3XKK(Opcode opcode)
{
    return Operand::split<Operand::XKK>(opcode);
};

/// @brief Encode opcode 4XKK
///
/// @param x  Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode encode4XKK(uint16_t x, uint16_t kk)
{
    return OPCODE_4XKK | Operand::join(Operand::XKK(x, kk));
}

/// @brief Decode opcode 4XKK
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XKK decode4XKK(Opcode opcode)
{
    return Operand::split<Operand::XKK>(opcode);
};

/// @brief Encode opcode 5XY0
///
/// @param x  Vx.
/// @param y  Vy.
/// @return Opcode.
inline Opcode encode5XY0(uint16_t x, uint16_t y)
{
    return OPCODE_5XY0 | Operand::join(Operand::XY(x, y));
}

/// @brief Decode opcode 5XY0
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XY decode5XY0(Opcode opcode)
{
    return Operand::split<Operand::XY>(opcode);
};

/// @brief Encode opcode 6XKK
///
/// @param x  Register Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode encode6XKK(uint16_t x, uint16_t kk)
{
    return OPCODE_6XKK | Operand::join(Operand::XKK(x, kk));
}

/// @brief Decode opcode 6XKK
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XKK decode6XKK(Opcode opcode)
{
    return Operand::split<Operand::XKK>(opcode);
}

/// @brief Encode opcode 7XKK
///
/// @param x  Register Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode encode7XKK(uint16_t x, uint16_t kk)
{
    return OPCODE_7XKK | Operand::join(Operand::XKK(x, kk));
}

/// @brief Decode opcode 7XKK
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XKK decode7XKK(Opcode opcode)
{
    return Operand::split<Operand::XKK>(opcode);
}

/// @brief Encode opcode 8XY0
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY0(uint16_t x, uint16_t y)
{
    return OPCODE_8XY0 | Operand::join(Operand::XY(x, y));
}

/// @brief Decode opcode 8XY0
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XY decode8XY0(Opcode opcode)
{
    return Operand::split<Operand::XY>(opcode);
}

/// @brief Encode opcode 8XY1
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY1(uint16_t x, uint16_t y)
{
    return OPCODE_8XY1 | Operand::join(Operand::XY(x, y));
}

/// @brief Decode opcode 8XY1
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XY decode8XY1(Opcode opcode)
{
    return Operand::split<Operand::XY>(opcode);
}

/// @brief Encode opcode 8XY2
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY2(uint16_t x, uint16_t y)
{
    return OPCODE_8XY2 | Operand::join(Operand::XY(x, y));
}

/// @brief Decode opcode 8XY2
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XY decode8XY2(Opcode opcode)
{
    return Operand::split<Operand::XY>(opcode);
}

/// @brief Encode opcode 8XY3
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY3(uint16_t x, uint16_t y)
{
    return OPCODE_8XY3 | Operand::join(Operand::XY(x, y));
}

/// @brief Decode opcode 8XY3
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XY decode8XY3(Opcode opcode)
{
    return Operand::split<Operand::XY>(opcode);
}

/// @brief Encode opcode 8XY4
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY4(uint16_t x, uint16_t y)
{
    return OPCODE_8XY4 | Operand::join(Operand::XY(x, y));
}

/// @brief Decode opcode 8XY4
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XY decode8XY4(Opcode opcode)
{
    return Operand::split<Operand::XY>(opcode);
}

/// @brief Encode opcode 8XY5
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY5(uint16_t x, uint16_t y)
{
    return OPCODE_8XY5 | Operand::join(Operand::XY(x, y));
}

/// @brief Decode opcode 8XY5
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XY decode8XY5(Opcode opcode)
{
    return Operand::split<Operand::XY>(opcode);
}

/// @brief Encode opcode 8XY6
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY6(uint16_t x, uint16_t y)
{
    return OPCODE_8XY6 | Operand::join(Operand::XY(x, y));
}

/// @brief Decode opcode 8XY6
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XY decode8XY6(Opcode opcode)
{
    return Operand::split<Operand::XY>(opcode);
}

/// @brief Encode opcode 8XY7
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XY7(uint16_t x, uint16_t y)
{
    return OPCODE_8XY7 | Operand::join(Operand::XY(x, y));
}

/// @brief Decode opcode 8XY7
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XY decode8XY7(Opcode opcode)
{
    return Operand::split<Operand::XY>(opcode);
}

/// @brief Encode opcode 8XYE
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode8XYE(uint16_t x, uint16_t y)
{
    return OPCODE_8XYE | Operand::join(Operand::XY(x, y));
}

/// @brief Decode opcode 8XYE
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XY decode8XYE(Opcode opcode)
{
    return Operand::split<Operand::XY>(opcode);
}

/// @brief Encode opcode 9XY0
///
/// @param x  Register Vx.
/// @param y  Register Vy.
/// @return Opcode.
inline Opcode encode9XY0(uint16_t x, uint16_t y)
{
    return OPCODE_9XY0 | Operand::join(Operand::XY(x, y));
}

/// @brief Decode opcode 9XY0
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XY decode9XY0(Opcode opcode)
{
    return Operand::split<Operand::XY>(opcode);
}

/// @brief Encode opcode ANNN
///
/// @param nnn Address.
/// @return Opcode.
inline Opcode encodeANNN(uint16_t nnn)
{
    return OPCODE_ANNN | Operand::join(Operand::NNN(nnn));
}

/// @brief Decode opcode ANNN
///
/// @param operands Operands.
/// @return Operands.
inline Operand::NNN decodeANNN(Opcode opcode)
{
    return Operand::split<Operand::NNN>(opcode);
}

/// @brief Encode opcode BNNN
///
/// @param nnn Address.
/// @return Opcode.
inline Opcode encodeBNNN(uint16_t nnn)
{
    return OPCODE_BNNN | Operand::join(Operand::NNN(nnn));
}

/// @brief Decode opcode BNNN
///
/// @param operands Operands.
/// @return Operands.
inline Operand::NNN decodeBNNN(Opcode opcode)
{
    return Operand::split<Operand::NNN>(opcode);
}

/// @brief Encode opcode CXKK
///
/// @param x  Vx.
/// @param kk Byte.
/// @return Opcode.
inline Opcode encodeCXKK(uint16_t x, uint16_t kk)
{
    return OPCODE_CXKK | Operand::join(Operand::XKK(x, kk));
}

/// @brief Decode opcode CXKK
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XKK decodeCXKK(Opcode opcode)
{
    return Operand::split<Operand::XKK>(opcode);
}

/// @brief Encode opcode DXYN
///
/// @param x  Vx.
/// @param y  Vy.
/// @param n  Address.
/// @return Opcode.
inline Opcode encodeDXYN(uint16_t x, uint16_t y, uint16_t n)
{
    return OPCODE_DXYN | Operand::join(Operand::XYN(x, y, n));
}

/// @brief Decode opcode DXYN
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::XYN decodeDXYN(Opcode opcode)
{
    return Operand::split<Operand::XYN>(opcode);
}

/// @brief Encode opcode EX9E
///
/// @param x  Vx.
/// @return Opcode.
inline Opcode encodeEX9E(uint16_t x)
{
    return OPCODE_EX9E | Operand::join(Operand::X(x));
}

/// @brief Decode opcode EX9E
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::X decodeEX9E(Opcode opcode)
{
    return Operand::split<Operand::X>(opcode);
}

/// @brief Encode opcode EXA1
///
/// @param x  Vx.
/// @return Opcode.
inline Opcode encodeEXA1(uint16_t x)
{
    return OPCODE_EXA1 | Operand::join(Operand::X(x));
}

/// @brief Decode opcode EXA1
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::X decodeEXA1(Opcode opcode)
{
    return Operand::split<Operand::X>(opcode);
}

/// @brief Encode opcode FX07.
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX07(uint16_t x)
{
    return OPCODE_FX07 | Operand::join(Operand::X(x));
}

/// @brief Decode opcode FX07.
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::X decodeFX07(Opcode opcode)
{
    return Operand::split<Operand::X>(opcode);
}

/// @brief Encode opcode FX0A
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX0A(uint16_t x)
{
    return OPCODE_FX0A | Operand::join(Operand::X(x));
}

/// @brief Decode opcode FX0A.
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::X decodeFX0A(Opcode opcode)
{
    return Operand::split<Operand::X>(opcode);
}

/// @brief Encode opcode FX15
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX15(uint16_t x)
{
    return OPCODE_FX15 | Operand::join(Operand::X(x));
}

/// @brief Decode opcode FX15.
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::X decodeFX15(Opcode opcode)
{
    return Operand::split<Operand::X>(opcode);
}

/// @brief Encode opcode FX18
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX18(uint16_t x)
{
    return OPCODE_FX18 | Operand::join(Operand::X(x));
}

/// @brief Decode opcode FX18.
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::X decodeFX18(Opcode opcode)
{
    return Operand::split<Operand::X>(opcode);
}

/// @brief Encode opcode FX1E
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX1E(uint16_t x)
{
    return OPCODE_FX1E | Operand::join(Operand::X(x));
}

/// @brief Decode opcode FX1E.
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::X decodeFX1E(Opcode opcode)
{
    return Operand::split<Operand::X>(opcode);
}

/// @brief Encode opcode FX29
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX29(uint16_t x)
{
    return OPCODE_FX29 | Operand::join(Operand::X(x));
}

/// @brief Decode opcode FX29.
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::X decodeFX29(Opcode opcode)
{
    return Operand::split<Operand::X>(opcode);
}

/// @brief Encode opcode FX33
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX33(uint16_t x)
{
    return OPCODE_FX33 | Operand::join(Operand::X(x));
}

/// @brief Decode opcode FX33.
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::X decodeFX33(Opcode opcode)
{
    return Operand::split<Operand::X>(opcode);
}

/// @brief Encode opcode FX55
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX55(uint16_t x)
{
    return OPCODE_FX55 | Operand::join(Operand::X(x));
}

/// @brief Decode opcode FX55.
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::X decodeFX55(Opcode opcode)
{
    return Operand::split<Operand::X>(opcode);
}

/// @brief Encode opcode FX65
///
/// @param x  Register Vx.
/// @return Opcode.
inline Opcode encodeFX65(uint16_t x)
{
    return OPCODE_FX65 | Operand::join(Operand::X(x));
}

/// @brief Decode opcode FX65.
///
/// @param opcode Opcode.
/// @return Operands.
inline Operand::X decodeFX65(Opcode opcode)
{
    return Operand::split<Operand::X>(opcode);
}

}  // opcode
}  // chip8

#endif  // CHIP8_OPCODE_HPP
