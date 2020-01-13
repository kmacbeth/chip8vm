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
#ifndef CHIP8_CPU_HPP
#define CHIP8_CPU_HPP

#include <cstdint>

namespace chip8 {

class Memory;

/// @brief CHIP-8 CPU
class Cpu
{
    public:
        Cpu(Memory & memory);

        void reset();
        void tick();
        void printTrace() const;

    private:
        static constexpr uint8_t  REG_COUNT = 16;
        static constexpr uint16_t PC_INCR = 2;


        class OpcodeDecoder
        {
            public:
                OpcodeDecoder(Cpu & cpu);

                void decode(uint16_t opcode);

            private:
                using OpcodeFunc = void (Cpu::*)();

                Cpu &      cpu_;
                OpcodeFunc dispatch_[2];
        };

        void opcodeLoadImmediate();
        void opcodeLoadRegister();

        Memory & memory_;
        uint16_t pc_;

        uint8_t  vx_[REG_COUNT];

        OpcodeDecoder opcodeDecoder_;
        uint16_t      opcode_;
};

}  // chip8

#endif  // CHIP8_CPU_HPP
