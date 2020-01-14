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
    , vx_()
    , i_(0)
    , opcodeDecoder_(*this)
    , opcode_(0x0000)
    , pc_(Memory::START_POINT)
    , sp_(0)
    , dt_(0)
    , st_(0)
{
}


/// @brief Reset cpu.
///
/// Reset CPU states, such as program counter and registers.
void Cpu::reset()
{
    pc_ = Memory::START_POINT;
    sp_ = 0;
    dt_ = 0;
    st_ = 0;

    std::memset(vx_, 0, sizeof(vx_));
}

/// @brief Process a cpu tick.
void Cpu::tick()
{
    opcode_ = memory_.loadOpcode(pc_);
    pc_ += PC_INCR;

    if (dt_ > 0)
    {
        --dt_;
    }

    // std::printf("\n================\nOpcode: %04X\n================\n", opcode_);

    opcodeDecoder_.decode(opcode_);

}

/// @brief Show a debug trace.
void Cpu::printTrace() const
{
    std::printf("\nGeneral Purpose Registers\n"
                "-------------------------\n");

    for (uint8_t regIndex = 0; regIndex < REG_COUNT; ++regIndex)
    {
        std::printf("V%01X = %02X", regIndex, vx_[regIndex]);
        std::printf(regIndex % 8 == 7 ? "\n" : " | ");
    }

    std::printf("\nSpecific Purpose Registers\n"
                "--------------------------\n");

    std::printf("PC = %04X | SP = %02X | I = %04X | DT = %02X | ST = %02X\n", pc_, sp_, i_, dt_, st_);
}

void Cpu::dumpRegisters()
{
    const size_t DUMP_START = 0xFE0;

    uint16_t address = DUMP_START;

    memory_.storeData(address, pc_);
    memory_.storeData(address + 2, i_);
    memory_.storeData(address + 4, sp_);
    memory_.storeData(address + 5, dt_);
    memory_.storeData(address + 6, st_);

    for (uint32_t i = 0; i < REG_COUNT; ++i)
    {
        memory_.storeData(address + 0x10 + i, vx_[i]);
    }
}

/// @brief Load a number to register Vx
///
/// Opcode 6xkk (LD Vx,byte)
void Cpu::opcodeLoadNumber()
{
    uint8_t regDest = (opcode_ & 0x0F00) >> 8;
    uint8_t number  = (opcode_ & 0x00FF);

    vx_[regDest] = number;
}

/// @brief Load register Vy to register Vx
///
/// Opcode 8xy0 (LD Vx,Vy)
void Cpu::opcodeLoadRegister()
{
    uint8_t regDest = (opcode_ >> 8) & 0xF;
    uint8_t regSrc  = (opcode_ >> 4) & 0xF;

    vx_[regDest] = vx_[regSrc];
}

/// @brief Load I register with 12-bit address
///
/// Opcode Annn (LD I,addr)
void Cpu::opcodeLoadIRegister()
{
    i_ = (opcode_ & 0xFFF);
}

/// @brief Load delay timer from register.
///
/// Opcode Fx15 (LD DT,Vx)
void Cpu::opcodeLoadDelayTimerFromRegister()
{
    uint8_t regSrc = (opcode_ >> 8) & 0xF;

    dt_ = vx_[regSrc];
}
/// @brief Load delay timer from register.
///
/// Opcode Fx07 (LD Vx,DT)
void Cpu::opcodeLoadRegisterFromDelayTimer()
{
    uint8_t regDest = (opcode_ >> 8) & 0xF;

    vx_[regDest] = dt_;
}

}  // chip8
