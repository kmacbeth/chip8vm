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
#ifndef CHIP8_CPUDEBUGGER_HPP
#define CHIP8_CPUDEBUGGER_HPP

#include <cpu.hpp>

namespace chip8 {

/// @brief Represent a CPU debugger.
class Debugger : public Cpu
{
    public:
        /// @brief Debug trace types.
        enum Traces : uint16_t
        {
            NONE      = 0x0,
            REGISTERS = 0x1,
            OPCODE    = 0x2,
            STACK     = 0x4,

            ALL       = 0x7
        };

        Debugger(std::shared_ptr<Cpu> cpu, std::shared_ptr<Memory> memory);
        ~Debugger();

        void setTraces(uint16_t traces) { traces_ = traces; }

        uint16_t getProgramCounter() const { return regContext_.pc; }
        uint8_t  getRegisterVx(uint16_t vxIndex) const { return regContext_.vx[vxIndex]; }
        uint8_t  getStackPointer() const { return regContext_.sp; }
        uint16_t getRegisterI() const { return regContext_.i; }
        uint8_t  getDelayTimer()const { return regContext_.dt; }
        uint8_t  getSoundTimer() const { return regContext_.st; }

        void tick(uint32_t tick) override;
        void reset() override;
        void update() override;

    private:
        Cpu::RegContext const& getRegContext() const override;
        opcode::Opcode getOpcode() const override;

        void traceRegContext();
        void traceOpcode();
        void traceStack();

        std::shared_ptr<Cpu> cpu_;
        std::shared_ptr<Memory> memory_;
        uint16_t traces_;

        Cpu::RegContext regContext_;
        opcode::Opcode opcode_;
};

}  // chip8

#endif  // CHIP8_CPUDEBUGGER_HPP
