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

/// @brief Construct a CHIP-8 cpu debugger.
///
/// @param cpu    Reference to cpu.
/// @param memory Reference to system memory.
Debugger::Debugger(Processor & cpu, Memory & memory)
    : Processor{}
    , cpu_{cpu}
    , memory_{memory}
    , traces_{Traces::NONE}
    , regContext_{}
    , opcode_(0)
{
}

/// @brief Destroy a CHIP-8 cpu debugger.
Debugger::~Debugger()
{
}

/// @brief Reset CPU.
void Debugger::reset()
{
    cpu_.reset();
}

/// @brief Request CPU tick
void Debugger::tick()
{
    cpu_.tick();

    opcode_ = getOpcode();
    regContext_ = getRegContext();

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

/// @brief Get CPU registers context.
///
/// @return Registers context.
Processor::RegContext const& Debugger::getRegContext() const
{
    return cpu_.getRegContext();
}

/// @brief Get opcode.
///
/// @return Opcode.
opcode::Opcode Debugger::getOpcode() const
{
    return cpu_.getOpcode();
}

/// @brief Trace opcode to console.
void Debugger::traceOpcode()
{
    std::printf("========================================\n");
    std::printf("OPCODE: %04X\n", static_cast<uint16_t>(opcode_));
}

/// @brief Trace registers context to console.
void Debugger::traceRegContext()
{
    std::printf("========================================\n");
    std::printf("  PC: 0x%04X  I:  0x%04X  SP: 0x%02X\n", regContext_.pc, regContext_.i, regContext_.sp);
    std::printf("  DT: 0x%02X    ST: 0x%02X\n", regContext_.dt, regContext_.st);
    std::printf("========================================\n");
    for (uint16_t i = 0; i < chip8::Cpu::REG_COUNT; ++i)
    {
        std::printf("  V%X: %02X  ", i, regContext_.vx[i]);
        std::printf("%s", ((i % 4)) == 3 ? "\n" : "");
    }
}

/// @brief Trace stack to console.
void Debugger::traceStack()
{
    std::printf("========================================\n");
    std::printf(" + STACK:\n");

    for (uint16_t i = 0; i < chip8::Processor::STACK_SIZE; ++i)
    {
        std::printf(" %2s %02X: %04X", ((i < (chip8::Processor::STACK_SIZE - 1)) ? "|-" : "`-"),
                    i, regContext_.stack[i]);
        std::puts((i + 1) == regContext_.sp ? " <-- SP" : "");
    }
}


}  // chip8
