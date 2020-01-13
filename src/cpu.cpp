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
#include <cstring>
#include <memory.hpp>
#include "cpu.hpp"

namespace chip8 {

/// @brief Construct an opcode decoder.
///
/// @param cpu Reference to the CPU instance.
Cpu::OpcodeDecoder::OpcodeDecoder(Cpu & cpu)
    : cpu_(cpu)
    , dispatch_()
{
    dispatch_[0] = &Cpu::opcodeLoadImmediate;
    dispatch_[1] = &Cpu::opcodeLoadRegister;
}

/// @brief Decode opcode.
///
/// @param opcode Opcode to decode and execute.
void Cpu::OpcodeDecoder::decode(uint16_t opcode)
{
    uint8_t index = 0;

    switch(opcode & 0xF000)
    {
        // LD Vx,byte
        case 0x6000:
            index = 0;
            break;

        // LD Vx,Vy
        case 0x8000:
            index = 1;

        default:
            break;
    }

    OpcodeFunc opcodeCall = dispatch_[index];

    (cpu_.*opcodeCall)();
}


/// @brief Construct a CPU instance.
///
/// @param memory Reference to memory (4K).
Cpu::Cpu(Memory & memory)
    : memory_(memory)
    , vx_()
    , pc_(Memory::START_POINT)
    , opcodeDecoder_(*this)
    , opcode_(0x0000)
{
}


/// @brief Reset cpu.
///
/// Reset CPU states, such as program counter and registers.
void Cpu::reset()
{
    pc_ = Memory::START_POINT;

    std::memset(vx_, 0, sizeof(vx_));
}

/// @brief Process a cpu tick.
void Cpu::tick()
{
    opcode_ = memory_.getOpcode(pc_);
    pc_ += PC_INCR;

    std::printf("Opcode: %04X\n", opcode_);

    opcodeDecoder_.decode(opcode_);
}

/// @brief Show a debug trace.
void Cpu::printTrace() const
{
    std::printf("Registers VX Traces\n"
                "-------------------\n");

    for (uint8_t regIndex = 0; regIndex < REG_COUNT; ++regIndex)
    {
        std::printf("V%01X = %02X", regIndex, vx_[regIndex]);
        std::printf(regIndex % 4 == 3 ? "\n" : " | ");
    }
}

/// @brief Load a number to register Vx
void Cpu::opcodeLoadImmediate()
{
    uint8_t regDest = (opcode_ & 0x0F00) >> 8;
    uint8_t number  = (opcode_ & 0x00FF);

    vx_[regDest] = number;
}

// @brief Load register Vy to register Vx
void Cpu::opcodeLoadRegister()
{
    uint8_t regDest = (opcode_ & 0x0F00) >> 8;
    uint8_t regSrc  = (opcode_ & 0x00F0) >> 4;

    vx_[regDest] = vx_[regSrc];
}

}  // chip8
