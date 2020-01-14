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
#ifndef CHIP8_MEMORY_HPP
#define CHIP8_MEMORY_HPP

#include <core.hpp>

namespace chip8 {

/// @brief Memory class.
class Memory
{
    public:
        /// @brief Total memory size.
        static constexpr uint16_t MEMORY_SIZE = 4096;
        /// @brief Program start point in memory.
        static constexpr uint16_t START_POINT = 0x200;

        void storeBytes(uint8_t * program, size_t size);
        void storeOpcodes(uint16_t * opcodes, uint16_t opcodeCount);

        void storeData(uint16_t address, uint8_t  data);
        void storeData(uint16_t address, uint16_t data);

        uint16_t loadOpcode(uint16_t address);
        uint8_t  loadData(uint16_t address);

    private:
        /// @brief Memory buffer in bytes.
        uint8_t memory_[MEMORY_SIZE];
};

}  // chip8

#endif  // CHIP8_MEMORY_HPP
