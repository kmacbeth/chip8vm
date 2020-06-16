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

#include <random>
#include <unordered_map>
#include <core.hpp>
#include <opcode.hpp>

namespace chip8 {

class Memory;
class Gpu;
class Keyboard;

/// @brief Represent a CHIP-8 CPU.
class Cpu
{
    public:
        /// @brief Program counter increment.
        static constexpr uint16_t PC_INCR = 2;
        /// @brief Total registers count.
        static constexpr uint8_t REG_COUNT = 16;
        /// @brief Stack size.
        static constexpr uint8_t STACK_SIZE = 16;
        /// @brief Program start point in memory.
        static constexpr uint16_t PROGRAM_START = 0x200;

        /// @brief Register context for debugging/testing
        struct RegContext
        {
            /// @brief Program counter.
            uint16_t pc;

            /// @brief General purpose registers.
            uint8_t  vx[REG_COUNT];

            /// @brief Stack pointer.
            uint8_t  sp;
            /// @brief Stack.
            uint16_t stack[STACK_SIZE];

            /// @brief I egister.
            uint16_t i;
            /// @brief Delay register.
            uint8_t  dt;
            /// @brief Sound register.
            uint8_t  st;
        };

        virtual ~Cpu() {}

        virtual void tick(uint32_t tick) = 0;
        virtual void reset() = 0;
        virtual void update() = 0;
        virtual void updateTimer() = 0;

        virtual RegContext const& getRegContext() const = 0;
        virtual opcode::Opcode    getOpcode() const = 0;
};

/// @brief Represent a CHIP-8 CPU implementation.
class CpuImpl : public Cpu
{
    public:
        CpuImpl(std::shared_ptr<Memory> memory,
                std::shared_ptr<Keyboard> keyboard,
                std::shared_ptr<Gpu> gpu);
        ~CpuImpl();

        virtual void tick(uint32_t tick) override { tick_ = tick; }
        virtual void reset() override;
        virtual void update() override;
        virtual void updateTimer () override;

        RegContext const& getRegContext() const override { return regs_; }
        opcode::Opcode getOpcode() const override { return opcode_; }

    private:
        void resetRegisters();

        /// @brief An opcode decoder.
        class OpcodeDecoder
        {
            public:
                OpcodeDecoder() = delete;
                OpcodeDecoder(CpuImpl * cpu);

                void decode(opcode::Opcode opcode);

            private:
                using OpcodeFunc = void (CpuImpl::*)();

                /// @brief Opcode dispatch table.
                static const std::unordered_map<opcode::Opcode, OpcodeFunc> opcodeTable_;

                /// @brief Reference to cpu instance.
                CpuImpl * cpu_;
        };

        void opcodeClearDisplay();
        void opcodeReturn();
        void opcodeJump();
        void opcodeCall();
        void opcodeSkipNextIfEquals();
        void opcodeSkipNextIfNotEquals();
        void opcodeSkipNextIfEqualsRegister();
        void opcodeLoadNumber();
        void opcodeAddNumber();
        void opcodeLoadRegister();
        void opcodeOrRegister();
        void opcodeAndRegister();
        void opcodeXorRegister();
        void opcodeAddRegister();
        void opcodeSubRegister();
        void opcodeShrRegister();
        void opcodeSubnRegister();
        void opcodeShlRegister();
        void opcodeSkipNextIfNotEqualsRegister();
        void opcodeLoadIRegister();
        void opcodeJumpOffset();
        void opcodeRandomNumber();
        void opcodeDraw();
        void opcodeSkipNextIfKeyEqualsRegister();
        void opcodeSkipNextIfKeyNotEqualsRegister();
        void opcodeLoadDelayTimerFromRegister();
        void opcodeLoadRegisterFromDelayTimer();
        void opcodeLoadSoundTimerFromRegister();
        void opcodeAddIRegister();
        void opcodeLoadIRegisterWithAddress();
        void opcodeStoreBinaryCodedDecimal();
        void opcodeStoreRegistersWithAddress();
        void opcodeLoadRegistersWithAddress();

        /// @brief Main memory instance.
        std::shared_ptr<Memory> memory_;
        /// @brief Keyboard
        std::shared_ptr<Keyboard> keyboard_;
        /// @brief GPU display.
        std::shared_ptr<Gpu> gpu_;

        /// @brief Register context.
        RegContext regs_;
        /// @brief Opcode decoder instance.
        OpcodeDecoder opcodeDecoder_;
        /// @brief Current opcode.
        opcode::Opcode opcode_;
        /// @brief CPU tick
        uint32_t tick_;
        /// @brief Delay Timer tick
        uint32_t delayTimerTick_;
        /// @brief Sound Timer tick
        uint32_t soundTimerTick_;

        /// @brief Random number generator.
        std::uniform_int_distribution<uint8_t> randomizer_;
        std::mt19937 bitGenerator_;
};

}  // chip8

#endif  // CHIP8_CPU_HPP
