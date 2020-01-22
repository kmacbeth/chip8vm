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
#include <cstring>
#include <core.hpp>
#include "memory.hpp"

namespace chip8 {

/// @brief Construct memory instance.
///
/// @param size Memory size.
Memory::Memory(size_t size)
    : memory_(size)
{
}

/// @brief Store buffer from 8-bit byte list.
///
/// @param startAddress Address of start point.
/// @param buffer       Reference to buffer.
void Memory::storeBuffer(uint16_t startAddress, Bytes const& buffer)
{
    std::memcpy(memory_.data() + startAddress, buffer.data(), buffer.size());
}

/// @brief Store program from 16-bit word list.
///
/// @param startAddress Address of start point.
/// @param buffer       Reference to buffer.
/// @param endian       Buffer data endianness.
void Memory::storeBuffer(uint16_t startAddress, Words const& buffer, Endian endian)
{
    for (uint16_t index = 0; index < buffer.size(); ++index)
    {
        uint16_t address = startAddress + 2 * index;

        switch (endian)
        {
            case Endian::LITTLE:
                memory_[address    ] = (buffer[index] >> 8) & 0xFF;
                memory_[address + 1] = buffer[index] & 0xFF;
                break;
            case Endian::BIG:
                memory_[address    ] = buffer[index] & 0xFF;
                memory_[address + 1] = (buffer[index] >> 8) & 0xFF;
                break;
        }
    }
}

/// @brief Store a byte.
///
/// @param address Address at which to store the byte.
/// @param byte    Byte to store.
void Memory::store(uint16_t address, uint8_t byte)
{
    memory_[address] = byte;
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
