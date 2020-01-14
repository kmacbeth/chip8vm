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

const Cpu::OpcodeDecoder::OpcodeFunc Cpu::OpcodeDecoder::dispatch_[] = {
    &Cpu::opcodeLoadNumber,
    &Cpu::opcodeLoadRegister,
    &Cpu::opcodeLoadIRegister,
    &Cpu::opcodeLoadDelayTimerFromRegister,
    &Cpu::opcodeLoadRegisterFromDelayTimer
};

/// @brief Construct an opcode decoder.
///
/// @param cpu Reference to the CPU instance.
Cpu::OpcodeDecoder::OpcodeDecoder(Cpu & cpu)
    : cpu_(cpu)
{
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
            break;

        // LD I,addr
        case 0xA000:
            index = 2;
            break;

        case 0xF000:
            switch (opcode & 0x00FF)
            {
                // LD DT,Vx
                case 0x0015:
                    index = 3;
                    break;

                // LD Vx,DT
                case 0x0007:
                    index = 4;
            }
            break;

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
    , regs_()
    , opcodeDecoder_(*this)
    , opcode_(0x0000)
{
    reset();
}


/// @brief Reset cpu.
///
/// Reset CPU states, such as program counter and registers.
void Cpu::reset()
{
    regs_.pc = Memory::START_POINT;
    std::memset(regs_.vx, 0, sizeof(regs_.vx));
    regs_.sp = 0;
    regs_.i  = 0;
    regs_.dt = 0;
    regs_.st = 0;
}

/// @brief Process a cpu tick.
void Cpu::tick()
{
    opcode_ = memory_.loadOpcode(regs_.pc);
    regs_.pc += PC_INCR;

    if (regs_.dt > 0)
    {
        --regs_.dt;
    }

    opcodeDecoder_.decode(opcode_);

}

/// @brief Dump CPU register context.
Cpu::RegContext Cpu::dumpRegContext()
{
    return regs_;
}

/// @brief Load a number to register Vx
///
/// Opcode 6xkk (LD Vx,byte)
void Cpu::opcodeLoadNumber()
{
    uint8_t regDest = (opcode_ & 0x0F00) >> 8;
    uint8_t number  = (opcode_ & 0x00FF);

    regs_.vx[regDest] = number;
}

/// @brief Load register Vy to register Vx
///
/// Opcode 8xy0 (LD Vx,Vy)
void Cpu::opcodeLoadRegister()
{
    uint8_t regDest = (opcode_ >> 8) & 0xF;
    uint8_t regSrc  = (opcode_ >> 4) & 0xF;

    regs_.vx[regDest] = regs_.vx[regSrc];
}

/// @brief Load I register with 12-bit address
///
/// Opcode Annn (LD I,addr)
void Cpu::opcodeLoadIRegister()
{
    regs_.i = (opcode_ & 0xFFF);
}

/// @brief Load delay timer from register.
///
/// Opcode Fx15 (LD DT,Vx)
void Cpu::opcodeLoadDelayTimerFromRegister()
{
    uint8_t regSrc = (opcode_ >> 8) & 0xF;

    regs_.dt = regs_.vx[regSrc];
}
/// @brief Load delay timer from register.
///
/// Opcode Fx07 (LD Vx,DT)
void Cpu::opcodeLoadRegisterFromDelayTimer()
{
    uint8_t regDest = (opcode_ >> 8) & 0xF;

    regs_.vx[regDest] = regs_.dt;
}

}  // chip8
