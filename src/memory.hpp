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

#include <array>
#include <vector>
#include <core.hpp>

namespace chip8 {

/// @brief Memory class.
class Memory
{
    public:
        using Bytes = std::vector<uint8_t>;
        using Words = std::vector<uint16_t>;

        enum class Endian { BIG, LITTLE };

        /// @brief Total memory size.
        static constexpr uint16_t MEMORY_SIZE = 4096;
        /// @brief Program start point in memory.
        static constexpr uint16_t START_POINT = 0x200;

        void storeProgram(Bytes & program);
        void storeProgram(Words & program, Endian endian);

        template<typename TYPE>
        TYPE load(uint16_t address);

    private:
        /// @brief Memory buffer in bytes.
        std::array<uint8_t, MEMORY_SIZE> memory_;
};

}  // chip8

#endif  // CHIP8_MEMORY_HPP
