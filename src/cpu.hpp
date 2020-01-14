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

#include <core.hpp>

namespace chip8 {

class Memory;

/// @brief Represent the CHIP-8 CPU.
class Cpu
{
    public:
        /// @brief Total registers count.
        static constexpr uint8_t REG_COUNT = 16;

        /// @brief Register context for debugging/testing
        struct RegContext
        {
            /// @brief Program counter.
            uint16_t pc;

            /// @brief General purpose registers.
            uint8_t  vx[REG_COUNT];
            /// @brief Stack pointer.
            uint8_t  sp;

            /// @brief I register.
            uint16_t i;
            /// @brief Delay register.
            uint8_t  dt;
            /// @brief Sound register.
            uint8_t  st;
        };

        Cpu(Memory & memory);

        void reset();
        void tick();

        // Custom debugging/testing
        RegContext dumpRegContext();

    private:
        /// @brief Program counter increment.
        static constexpr uint16_t PC_INCR = 2;
        /// @brief Stack size.
        static constexpr uint8_t STACK_SIZE = 16;

        /// @brief An opcode decoder.
        class OpcodeDecoder
        {
            public:
                OpcodeDecoder(Cpu & cpu);

                void decode(uint16_t opcode);

            private:
                using OpcodeFunc = void (Cpu::*)();

                /// @brief Reference to cpu instance.
                Cpu &      cpu_;
                /// @brief Opcode dispatch table.
                static const OpcodeFunc dispatch_[];
        };

        void opcodeLoadNumber();
        void opcodeLoadRegister();
        void opcodeLoadIRegister();
        void opcodeLoadDelayTimerFromRegister();
        void opcodeLoadRegisterFromDelayTimer();
        void opcodeLoadSoundTimerFromRegister();

        /// @brief Main memory instance.
        Memory & memory_;
        /// @brief Register context.
        RegContext regs_;

        /// @brief Opcode decoder instance.
        OpcodeDecoder opcodeDecoder_;
        /// @brief Current opcode
        uint16_t opcode_;
};

}  // chip8

#endif  // CHIP8_CPU_HPP
