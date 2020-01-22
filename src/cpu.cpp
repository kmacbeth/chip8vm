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
#include <gpu.hpp>
#include "cpu.hpp"


namespace chip8 {

const std::unordered_map<opcode::Opcode, Cpu::OpcodeDecoder::OpcodeFunc> Cpu::OpcodeDecoder::opcodeTable_ = {
    { opcode::OPCODE_00E0, &Cpu::opcodeClearDisplay },
    { opcode::OPCODE_00EE, &Cpu::opcodeReturn },
    { opcode::OPCODE_1NNN, &Cpu::opcodeJump },
    { opcode::OPCODE_2NNN, &Cpu::opcodeCall },
    { opcode::OPCODE_3XKK, &Cpu::opcodeSkipNextIfEquals },
    { opcode::OPCODE_4XKK, &Cpu::opcodeSkipNextIfNotEquals },
    { opcode::OPCODE_5XY0, &Cpu::opcodeSkipNextIfEqualsRegister },
    { opcode::OPCODE_6XKK, &Cpu::opcodeLoadNumber },
    { opcode::OPCODE_8XY0, &Cpu::opcodeLoadRegister },
    { opcode::OPCODE_ANNN, &Cpu::opcodeLoadIRegister },
    { opcode::OPCODE_FX07, &Cpu::opcodeLoadRegisterFromDelayTimer },
    { opcode::OPCODE_FX15, &Cpu::opcodeLoadDelayTimerFromRegister },
    { opcode::OPCODE_FX18, &Cpu::opcodeLoadSoundTimerFromRegister },
};

/// @brief Construct an opcode decoder.
///
/// @param cpu Reference to the CPU instance.
Cpu::OpcodeDecoder::OpcodeDecoder(Cpu & cpu)
    : cpu_{cpu}
{
}

/// @brief Decode opcode.
///
/// @param opcode Opcode to decode and execute.
void Cpu::OpcodeDecoder::decode(opcode::Opcode opcode)
{
    uint16_t decodedOpcode = opcode & 0xF000;

    if (decodedOpcode == 0xF000 || decodedOpcode == 0x0000)
    {
        decodedOpcode |= (opcode & 0x00FF);
    }
    else if (decodedOpcode == 0x5000)
    {
        decodedOpcode |= (opcode & 0x000F);
    }

    auto opcodeIterator = opcodeTable_.find(decodedOpcode);

    if (opcodeIterator != opcodeTable_.end())
    {
        (cpu_.*(opcodeIterator->second))();
    }
}


/// @brief Construct a CPU instance.
///
/// @param memory Reference to memory.
/// @param gpu    Reference to GPU displau.
Cpu::Cpu(Memory & memory, Gpu & gpu)
    : memory_{memory}
    , gpu_{gpu}
    , regs_{}
    , opcodeDecoder_{*this}
    , opcode_{0x0000}
{
    reset();
}


/// @brief Reset cpu.
///
/// Reset CPU states, such as program counter and registers.
void Cpu::reset()
{
    regs_.pc = SYSTEM_START_POINT;
    std::memset(regs_.vx, 0, sizeof(regs_.vx));
    regs_.sp = 0;
    regs_.i  = 0;
    regs_.dt = 0;
    regs_.st = 0;
}

/// @brief Process a cpu tick.
void Cpu::tick()
{
    opcode_ = memory_.load<opcode::Opcode>(regs_.pc);
    regs_.pc += PC_INCR;

    if (regs_.dt > 0)
    {
        --regs_.dt;
    }

    if (regs_.st > 0)
    {
        --regs_.st;
    }

    opcodeDecoder_.decode(opcode_);
}

/// @brief Dump CPU register context.
Cpu::RegContext Cpu::dumpRegContext()
{
    return regs_;
}

/// @brief Clear display.
///
/// Opcode 00E0 (CLS)
void Cpu::opcodeClearDisplay()
{
    gpu_.clearFrameBuffer();
}

/// @brief Return from subroutine.
///
/// Opcode 00EE (RET)
void Cpu::opcodeReturn()
{
    if (regs_.sp > 0)
    {
        --regs_.sp;
    }

    regs_.pc = regs_.stack[regs_.sp];
}

/// @brief Jump to location.
///
/// Opcode 1NNN (jp addr)
void Cpu::opcodeJump()
{
    regs_.pc = opcode::decode1NNN(opcode_).nnn;
}

/// @brief Return from subroutine.
///
/// Opcode 2NNN (call addr)
void Cpu::opcodeCall()
{
    regs_.stack[regs_.sp++] = regs_.pc;
    regs_.pc = opcode::decode2NNN(opcode_).nnn;
}

/// @brief Skip next opcode if equals byte.
///
/// Opcode 3XKK (se Vx,byte)
void Cpu::opcodeSkipNextIfEquals()
{
    auto op = opcode::decode3XKK(opcode_);

    if (regs_.vx[op.x] == op.kk)
    {
        regs_.pc += 2;
    }
}

/// @brief Skip next opcode if not equals byte.
///
/// Opcode 4XKK (sne Vx,byte)
void Cpu::opcodeSkipNextIfNotEquals()
{
    auto op = opcode::decode4XKK(opcode_);

    if (regs_.vx[op.x] != op.kk)
    {
        regs_.pc += 2;
    }
}

/// @brief Skip next opcode if Vx register equals Vy register.
///
/// Opcode 5YX0 (se Vx,Vy)
void Cpu::opcodeSkipNextIfEqualsRegister()
{
    auto op = opcode::decode5XY0(opcode_);

    if (regs_.vx[op.x] == regs_.vx[op.y])
    {
        regs_.pc += 2;
    }
}

/// @brief Load a number to register Vx
///
/// Opcode 6xkk (LD Vx,byte)
void Cpu::opcodeLoadNumber()
{
    auto op = opcode::decode6XKK(opcode_);

    regs_.vx[op.x] = op.kk;
}

/// @brief Load register Vy to register Vx
///
/// Opcode 8xy0 (LD Vx,Vy)
void Cpu::opcodeLoadRegister()
{
    auto op = opcode::decode8XY0(opcode_);

    regs_.vx[op.x] = regs_.vx[op.y];
}

/// @brief Load I register with 12-bit address
///
/// Opcode Annn (LD I,addr)
void Cpu::opcodeLoadIRegister()
{
    regs_.i = opcode::decodeANNN(opcode_).nnn;
}

/// @brief Load delay timer from register.
///
/// Opcode Fx15 (LD DT,Vx)
void Cpu::opcodeLoadDelayTimerFromRegister()
{
    auto op = opcode::decodeFX15(opcode_);

    regs_.dt = regs_.vx[op.x];
}
/// @brief Load register from delay timer.
///
/// Opcode Fx07 (LD Vx,DT)
void Cpu::opcodeLoadRegisterFromDelayTimer()
{
    auto op = opcode::decodeFX07(opcode_);

    regs_.vx[op.x] = regs_.dt;
}

/// @brief Load sound timer from register.
///
/// Opcode Fx18 (LD ST,Vx)
void Cpu::opcodeLoadSoundTimerFromRegister()
{
    uint8_t regSrc = (opcode_ >> 8) & 0xF;

    auto op = opcode::decodeFX18(opcode_);

    regs_.st = regs_.vx[op.x];
}

}  // chip8
