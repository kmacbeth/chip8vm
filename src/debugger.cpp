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
#include "debugger.hpp"

namespace chip8 {

std::string opcodeName[] = {


};


/// @brief Construct a CHIP-8 cpu debugger.
///
/// @param cpu    Reference to cpu.
/// @param memory Reference to system memory.
Debugger::Debugger(std::shared_ptr<Cpu> cpu, std::shared_ptr<Memory> memory)
    : Cpu{}
    , cpu_{ cpu }
    , memory_{ memory }
    , traces_{ Traces::NONE }
    , regContext_{}
    , opcode_{ 0 }
{
}

/// @brief Destroy a CHIP-8 cpu debugger.
Debugger::~Debugger()
{
}

/// @brief Set tick to CPU.
void Debugger::tick(uint32_t tick)
{
    cpu_->tick(tick);
}

/// @brief Reset CPU.
void Debugger::reset()
{
    cpu_->reset();
}

/// @brief Request CPU update
void Debugger::update()
{
    cpu_->update();

    opcode_ = getOpcode();
    regContext_ = getRegContext();

    if (traces_)
    {
        std::printf("========================================\n");

        if (traces_ & Traces::OPCODE)
        {
            traceOpcode();
        }

        if (traces_ & Traces::REGISTERS)
        {
            traceRegContext();
        }

        if (traces_ & Traces::STACK)
        {
            traceStack();
        }
    }
}

/// @brief Request CPU timer update
void Debugger::updateTimer()
{
    cpu_->updateTimer();
}

/// @brief Get CPU registers context.
///
/// @return Registers context.
Cpu::RegContext const& Debugger::getRegContext() const
{
    return cpu_->getRegContext();
}

/// @brief Get opcode.
///
/// @return Opcode.
opcode::Opcode Debugger::getOpcode() const
{
    return cpu_->getOpcode();
}

/// @brief Trace opcode to console.
void Debugger::traceOpcode()
{
    std::printf(" OPCODE: %04X\n", static_cast<uint16_t>(opcode_));
}

/// @brief Trace registers context to console.
void Debugger::traceRegContext()
{
    std::printf(" PC: %04X  I:  %04X  SP: %02X  DT: %02X  ST: %02X\n",
                regContext_.pc, regContext_.i, regContext_.sp, regContext_.dt, regContext_.st);

    for (uint16_t i = 0; i < chip8::Cpu::REG_COUNT; ++i)
    {
        std::printf(" V%X: %02X  ", i, regContext_.vx[i]);
    }
    std::puts("");
}

/// @brief Trace stack to console.
void Debugger::traceStack()
{
    std::printf(" + STACK:\n");

    for (uint16_t i = 0; i < chip8::Cpu::STACK_SIZE; ++i)
    {
        std::printf(" %2u   ", i);
    }
    std::puts("");

    for (uint16_t i = 0; i < chip8::Cpu::STACK_SIZE; ++i)
    {
        std::printf(" %04X ", regContext_.stack[i]);
    }
    std::puts("");
}


}  // chip8
