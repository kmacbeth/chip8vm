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
#include "cpu_debugger.hpp"

namespace chip8 {

/// @brief Construct a CHIP-8 cpu debugger.
///
/// @param memory Reference to system memory.
/// @param gpu    Reference to gpu display.
CpuDebugger::CpuDebugger(Memory & memory, Gpu & gpu)
    : Cpu{memory, gpu}
    , traces_{Traces::NONE}
    , regContext_{}
    , opcode_(0)
{
}

/// @brief Request CPU tick
void CpuDebugger::tick()
{
    Cpu::tick();

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

/// @brief Trace opcode to console.
void CpuDebugger::traceOpcode()
{
    std::printf("========================\n");
    std::printf("OPCODE: %04X\n", static_cast<uint16_t>(opcode_));
}

/// @brief Trace registers context to console.
void CpuDebugger::traceRegContext()
{
    std::printf("========================\n");
    std::printf("PC: %02X\n", regContext_.pc);
    for (uint16_t i = 0; i < chip8::Cpu::REG_COUNT; ++i)
    {
        std::printf("V%u: %01X\n", i, regContext_.vx[i]);
    }
    std::printf("SP: %02X\n", regContext_.sp);
    std::printf("I : %02X\n", regContext_.i);
    std::printf("DT: %01X\n", regContext_.dt);
    std::printf("ST: %01X\n", regContext_.st);
}

/// @brief Trace stack to console.
void CpuDebugger::traceStack()
{
    std::printf("========================\n");
    std::printf("STACK:\n");
    for (uint16_t i = 0; i < chip8::Cpu::STACK_SIZE; ++i)
    {
        std::printf("%02X: %04X", i, regContext_.stack[i]);
        if ((i + 1) == regContext_.sp)
        {
            std::puts(" <-- SP");
        }
        else
        {
            std::puts("");
        }
    }
}


}  // chip8
