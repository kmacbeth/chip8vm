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
#include "memory.hpp"

namespace chip8 {

void Memory::loadFromFile(const char * filename)
{
}

void Memory::loadFromList(uint16_t * opcodeList, uint16_t count)
{
    std::memcpy(memory_ + START_POINT, opcodeList, sizeof(opcodeList) * count);
}

uint16_t Memory::getOpcode(uint16_t address)
{
    uint16_t opcode =
        (static_cast<uint16_t>(memory_[address]) << 8) |
        (static_cast<uint16_t>(memory_[address + 1]));

    return opcode;
}

uint8_t Memory::getData(uint16_t address)
{
    return 0;
}

}  // chip8
