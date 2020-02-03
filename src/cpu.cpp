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
#include <algorithm>
#include <memory.hpp>
#include <gpu.hpp>
#include "cpu.hpp"


namespace chip8 {

const std::unordered_map<opcode::Opcode, CpuImpl::OpcodeDecoder::OpcodeFunc> CpuImpl::OpcodeDecoder::opcodeTable_ = {
    { opcode::OPCODE_00E0, &CpuImpl::opcodeClearDisplay },
    { opcode::OPCODE_00EE, &CpuImpl::opcodeReturn },
    { opcode::OPCODE_1NNN, &CpuImpl::opcodeJump },
    { opcode::OPCODE_2NNN, &CpuImpl::opcodeCall },
    { opcode::OPCODE_3XKK, &CpuImpl::opcodeSkipNextIfEquals },
    { opcode::OPCODE_4XKK, &CpuImpl::opcodeSkipNextIfNotEquals },
    { opcode::OPCODE_5XY0, &CpuImpl::opcodeSkipNextIfEqualsRegister },
    { opcode::OPCODE_6XKK, &CpuImpl::opcodeLoadNumber },
    { opcode::OPCODE_7XKK, &CpuImpl::opcodeAddNumber },
    { opcode::OPCODE_8XY0, &CpuImpl::opcodeLoadRegister },
    { opcode::OPCODE_8XY1, &CpuImpl::opcodeOrRegister },
    { opcode::OPCODE_8XY2, &CpuImpl::opcodeAndRegister },
    { opcode::OPCODE_8XY3, &CpuImpl::opcodeXorRegister },
    { opcode::OPCODE_8XY4, &CpuImpl::opcodeAddRegister },
    { opcode::OPCODE_8XY5, &CpuImpl::opcodeSubRegister },
    { opcode::OPCODE_8XY6, &CpuImpl::opcodeShrRegister },
    { opcode::OPCODE_8XY7, &CpuImpl::opcodeSubnRegister },
    { opcode::OPCODE_8XYE, &CpuImpl::opcodeShlRegister },
    { opcode::OPCODE_9XY0, &CpuImpl::opcodeSkipNextIfNotEqualsRegister },
    { opcode::OPCODE_ANNN, &CpuImpl::opcodeLoadIRegister },
    { opcode::OPCODE_BNNN, &CpuImpl::opcodeJumpOffset },
    { opcode::OPCODE_CXKK, &CpuImpl::opcodeRandomNumber },
    { opcode::OPCODE_DXYN, &CpuImpl::opcodeDraw },
    { opcode::OPCODE_FX07, &CpuImpl::opcodeLoadRegisterFromDelayTimer },
    { opcode::OPCODE_FX15, &CpuImpl::opcodeLoadDelayTimerFromRegister },
    { opcode::OPCODE_FX18, &CpuImpl::opcodeLoadSoundTimerFromRegister },
};

/// @brief Construct an opcode decoder.
///
/// @param cpu Reference to the CPU instance.
CpuImpl::OpcodeDecoder::OpcodeDecoder(CpuImpl & cpu)
    : cpu_{cpu}
{
}

/// @brief Decode opcode.
///
/// @param opcode Opcode to decode and execute.
void CpuImpl::OpcodeDecoder::decode(opcode::Opcode opcode)
{
    uint16_t decodedOpcode = opcode & 0xF000;

    if (decodedOpcode == 0x0000 || decodedOpcode == 0xE000 || decodedOpcode == 0xF000)
    {
        decodedOpcode |= (opcode & 0x00FF);
    }
    else if (decodedOpcode == 0x5000 | decodedOpcode == 0x8000 || decodedOpcode == 0x9000)
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
CpuImpl::CpuImpl(Memory & memory, std::shared_ptr<Gpu> const& gpu)
    : memory_{memory}
    , gpu_{gpu}
    , regs_{}
    , opcodeDecoder_{*this}
    , opcode_{0x0000}
    , randomizer_{}
    , bitGenerator_{}
{
    resetRegisters();
}

/// @brief Destroy a CPU instance.
CpuImpl::~CpuImpl()
{
}

/// @brief Reset cpu.
///
/// Reset CPU states, such as program counter and registers.
void CpuImpl::reset()
{
    resetRegisters();
}

/// @brief Process a cpu tick.
void CpuImpl::tick()
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

/// @brief Reset CPU registers
/// Reset CPU states, such as program counter and registers.
void CpuImpl::resetRegisters()
{
    regs_.pc = PROGRAM_START;
    std::fill(regs_.vx, regs_.vx + sizeof(regs_.vx), 0);
    regs_.sp = 0;
    regs_.i  = 0;
    regs_.dt = 0;
    regs_.st = 0;
}

/// @brief Clear display.
///
/// Opcode 00E0 (CLS)
void CpuImpl::opcodeClearDisplay()
{
    gpu_->clearFramebuffer();
}

/// @brief Return from subroutine.
///
/// Opcode 00EE (RET)
void CpuImpl::opcodeReturn()
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
void CpuImpl::opcodeJump()
{
    regs_.pc = opcode::decode1NNN(opcode_).nnn;
}

/// @brief Return from subroutine.
///
/// Opcode 2NNN (call addr)
void CpuImpl::opcodeCall()
{
    regs_.stack[regs_.sp++] = regs_.pc;
    regs_.pc = opcode::decode2NNN(opcode_).nnn;
}

/// @brief Skip next opcode if equals byte.
///
/// Opcode 3XKK (se Vx,byte)
void CpuImpl::opcodeSkipNextIfEquals()
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
void CpuImpl::opcodeSkipNextIfNotEquals()
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
void CpuImpl::opcodeSkipNextIfEqualsRegister()
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
void CpuImpl::opcodeLoadNumber()
{
    auto op = opcode::decode6XKK(opcode_);

    regs_.vx[op.x] = op.kk;
}

/// @brief Add a number to register Vx
///
/// Opcode 7xkk (ADD Vx,byte)
void CpuImpl::opcodeAddNumber()
{
    auto op = opcode::decode7XKK(opcode_);

    regs_.vx[op.x] += op.kk;
}

/// @brief Load register Vy to register Vx
///
/// Opcode 8xy0 (LD Vx,Vy)
void CpuImpl::opcodeLoadRegister()
{
    auto op = opcode::decode8XY0(opcode_);

    regs_.vx[op.x] = regs_.vx[op.y];
}

/// @brief Or register Vy to register Vx
///
/// Opcode 8xy1 (OR Vx,Vy)
void CpuImpl::opcodeOrRegister()
{
    auto op = opcode::decode8XY1(opcode_);

    regs_.vx[op.x] |= regs_.vx[op.y];
}

/// @brief And register Vy to register Vx
///
/// Opcode 8xy2 (AND Vx,Vy)
void CpuImpl::opcodeAndRegister()
{
    auto op = opcode::decode8XY2(opcode_);

    regs_.vx[op.x] &= regs_.vx[op.y];
}

/// @brief Xor register Vy to register Vx
///
/// Opcode 8xy3 (XOR Vx,Vy)
void CpuImpl::opcodeXorRegister()
{
    auto op = opcode::decode8XY3(opcode_);

    regs_.vx[op.x] ^= regs_.vx[op.y];
}

/// @brief Add register Vy to register Vx
///
/// Opcode 8xy4 (ADD Vx,Vy)
void CpuImpl::opcodeAddRegister()
{
    auto op = opcode::decode8XY4(opcode_);

    uint16_t sum = regs_.vx[op.x] + regs_.vx[op.y];

    regs_.vx[0xF] = ((sum & 0xFF00) != 0);
    regs_.vx[op.x] = (sum & 0xFF);
}

/// @brief Sub register Vy to register Vx
///
/// Opcode 8xy5 (SUB Vx,Vy)
void CpuImpl::opcodeSubRegister()
{
    auto op = opcode::decode8XY5(opcode_);

    uint16_t difference = regs_.vx[op.x] - regs_.vx[op.y];

    regs_.vx[0xF] = (regs_.vx[op.x] > regs_.vx[op.y]);
    regs_.vx[op.x] = (difference & 0xFF);
}

/// @brief Shift right register Vy to register Vx
///
/// Opcode 8xy6 (SHR Vx,Vy)
void CpuImpl::opcodeShrRegister()
{
    auto op = opcode::decode8XY6(opcode_);

    regs_.vx[0xF] = regs_.vx[op.y] & 0x1;
    regs_.vx[op.x] = regs_.vx[op.y] >> 1;
}

/// @brief Sub reverse register Vx to register Vy
///
/// Opcode 8xy7 (SUBN Vx,Vy)
void CpuImpl::opcodeSubnRegister()
{
    auto op = opcode::decode8XY7(opcode_);

    uint16_t difference = regs_.vx[op.y] - regs_.vx[op.x];

    regs_.vx[0xF] = (regs_.vx[op.y] > regs_.vx[op.x]);
    regs_.vx[op.x] = (difference & 0xFF);
}

/// @brief Shift left register Vy to register Vx
///
/// Opcode 8xyE (SHL Vx,Vy)
void CpuImpl::opcodeShlRegister()
{
    auto op = opcode::decode8XY6(opcode_);

    regs_.vx[0xF] = regs_.vx[op.y] & 0x80;
    regs_.vx[op.x] = regs_.vx[op.y] << 1;
}

/// @brief Skip next opcode if Vx not equals Vy.
///
/// Opcode 9XY0 (sne Vx,Vy)
void CpuImpl::opcodeSkipNextIfNotEqualsRegister()
{
    auto op = opcode::decode9XY0(opcode_);

    if (regs_.vx[op.x] != regs_.vx[op.y])
    {
        regs_.pc += 2;
    }
}

/// @brief Load I register with 12-bit address
///
/// Opcode Annn (LD I,addr)
void CpuImpl::opcodeLoadIRegister()
{
    regs_.i = opcode::decodeANNN(opcode_).nnn;
}

/// @brief Jump to address with offset.
///
/// Opcode BNNN (JP V0,nnn)
void CpuImpl::opcodeJumpOffset()
{
    auto op = opcode::decodeBNNN(opcode_);

    regs_.pc = op.nnn + regs_.vx[0];
}

/// @brief Random number at register Vx.
///
/// Opcode Cxkk (RND Vx,byte)
void CpuImpl::opcodeRandomNumber()
{
    auto op = opcode::decodeCXKK(opcode_);

    uint8_t number = randomizer_(bitGenerator_); 

    regs_.vx[op.x] = number & op.kk;
}

/// @brief Draw sprite to gpu framebuffer.
///
/// Opcode Dxyn (DRW Vx,Vy,nibble)
void CpuImpl::opcodeDraw()
{
    auto op = opcode::decodeDXYN(opcode_);

    auto sprite = Memory::Bytes{};

    for (size_t offset = 0; offset < op.n; ++offset)
    {
        sprite.push_back(memory_.load<uint8_t>(regs_.i + offset));
    }

    if (gpu_->drawSprite(op.x, op.y, sprite))
    {
        regs_.vx[0xF] = 0x1;
    }
}

/// @brief Load delay timer from register.
///
/// Opcode Fx15 (LD DT,Vx)
void CpuImpl::opcodeLoadDelayTimerFromRegister()
{
    auto op = opcode::decodeFX15(opcode_);

    regs_.dt = regs_.vx[op.x];
}

/// @brief Load register from delay timer.
///
/// Opcode Fx07 (LD Vx,DT)
void CpuImpl::opcodeLoadRegisterFromDelayTimer()
{
    auto op = opcode::decodeFX07(opcode_);

    regs_.vx[op.x] = regs_.dt;
}

/// @brief Load sound timer from register.
///
/// Opcode Fx18 (LD ST,Vx)
void CpuImpl::opcodeLoadSoundTimerFromRegister()
{
    uint8_t regSrc = (opcode_ >> 8) & 0xF;

    auto op = opcode::decodeFX18(opcode_);

    regs_.st = regs_.vx[op.x];
}

}  // chip8
