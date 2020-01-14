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
#include <cstdio>
#include <cstring>
#include "memory.hpp"

namespace chip8 {

/// @brief Store program from 8-bit byte list.
///
/// @param program Reference to program buffer.
void Memory::storeProgram(Bytes & program)
{
    std::memcpy(memory_.data() + START_POINT, program.data(), program.size());
}

/// @brief Store program from 16-bit word list.
///
/// @param program Reference to program buffer.
void Memory::storeProgram(Words & program, Endian endian)
{
    for (uint16_t index = 0; index < program.size(); ++index)
    {
        uint16_t address = START_POINT + 2 * index;

        switch (endian)
        {
            case Endian::LITTLE:
                memory_[address    ] = (program[index] >> 8) & 0xFF;
                memory_[address + 1] = program[index] & 0xFF;
                break;
            case Endian::BIG:
                memory_[address    ] = program[index] & 0xFF;
                memory_[address + 1] = (program[index] >> 8) & 0xFF;
                break;
        }
    }
}

/// @brief Load 16-bit word from memory address.
///
/// @param address Memory address to load as opcode.
/// @return Opcode converted from big-endian to little-endian.
template<>
uint16_t Memory::load(uint16_t address)
{
    uint16_t opcode = 0x0000;

    opcode |= static_cast<uint16_t>(memory_[address++]) << 8;
    opcode |= static_cast<uint16_t>(memory_[address]);

    return opcode;
}

/// @brief Load data from memory address.
///
/// @param address Memory address to load data.
/// @return Data byte.
template<>
uint8_t Memory::load(uint16_t address)
{
    return memory_[address];
}

}  // chip8
