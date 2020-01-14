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

/// @brief Store bytes to memory.
///
/// @param program Program buffer to copy to memory.
/// @param size    Program buffer size.
void Memory::storeBytes(uint8_t * program, size_t size)
{
    std::memcpy(memory_ + START_POINT, program, size);
}

/// @brief Store opcodes.
///
/// @param opcodes     Store opcodes list to memory.
/// @param opcodeCount Opcode count from opcodes list.
void Memory::storeOpcodes(uint16_t * opcodes, uint16_t opcodeCount)
{
    for (uint16_t index = 0; index < opcodeCount; ++index)
    {
        uint16_t address = START_POINT + 2 * index;

        memory_[address    ] = (opcodes[index] >> 8) & 0xFF;
        memory_[address + 1] = opcodes[index] & 0xFF;
    }
}

void Memory::storeData(uint16_t address, uint8_t data)
{
    memory_[address] = data;
}

void Memory::storeData(uint16_t address, uint16_t data)
{
    memory_[address++] = data & 0xFF;
    memory_[address  ] = (data >> 8) & 0xFF;
}

/// @brief Load opcode from memory address.
///
/// @param address Memory address to load as opcode.
/// @return Opcode converted from big-endian to little-endian.
uint16_t Memory::loadOpcode(uint16_t address)
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
uint8_t Memory::loadData(uint16_t address)
{
    return memory_[address];
}

}  // chip8
