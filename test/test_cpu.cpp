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
#include <catch2/catch.hpp>
#include <vector>

#include <memory.hpp>
#include <cpu.hpp>

using OpcodeList = chip8::Memory::Words;

class Chip8TestVm
{
    public:
        Chip8TestVm()
            : memory_()
            , cpu_(memory_)
            , cpuRegCtx_()
        {
        }

        void loadCode(OpcodeList & opcodes)
        {
            memory_.storeProgram(opcodes, chip8::Memory::Endian::LITTLE);
        }

        void run()
        {
            cpu_.tick();
            cpuRegCtx_ = cpu_.dumpRegContext();
        }

        chip8::Cpu::RegContext const& getCpuRegCtx() const
        {
            return cpuRegCtx_;
        }

    private:
        chip8::Memory memory_;
        chip8::Cpu cpu_;
        chip8::Cpu::RegContext cpuRegCtx_;

};


TEST_CASE("Test CPU opcodes", "[cpu][opcode]")
{
    Chip8TestVm vm;

    SECTION("Load number to Vx register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0xAB;

        uint16_t loadNumberAB = chip8::opcode6XKK(vxIndex, expectedByte);

        OpcodeList opcodes = {
            loadNumberAB   // LD Vx,byte
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == expectedByte);
    }

    SECTION("Load Vy register to Vx register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0xAB;

        uint16_t loadNumberAB = chip8::opcode6XKK(vxIndex, expectedByte);
        uint16_t loadReg2Reg  = chip8::opcode8XY0(vxIndex + 1, vxIndex);

        OpcodeList opcodes = {
            loadNumberAB,  // LD Vx,byte
            loadReg2Reg    // LD Vx,Vy
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == expectedByte);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[(vxIndex + 1) & 0xF] == expectedByte);
    }

    SECTION("Load address to I register")
    {
        uint16_t loadI123 = chip8::opcodeANNN(0x123);

        OpcodeList opcodes = {
            loadI123   // LD I,123
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().i == 0x123);
    }

    SECTION("Load Vx register to DT register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0x10;

        uint16_t loadNumber10 = chip8::opcode6XKK(vxIndex, expectedByte);
        uint16_t loadReg2DelayTimer = chip8::opcodeFX15(vxIndex);

        OpcodeList opcodes = {
            loadNumber10,        // LD Vx,0x10
            loadReg2DelayTimer,  // LD DT,Vx
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == expectedByte);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().dt == expectedByte);
    }

    SECTION("Load DT register to Vx register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0x10;

        uint16_t loadNumber10 = chip8::opcode6XKK(vxIndex, expectedByte);
        uint16_t loadDelayTimer2Reg = chip8::opcodeFX07(vxIndex);

        OpcodeList opcodes = {
            loadNumber10,        // LD Vx,0x10
            loadDelayTimer2Reg,  // LD DT,Vx
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == expectedByte);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == 0x00);
    }

    SECTION("Load Vx to ST register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0x10;

        uint16_t loadNumber10 = chip8::opcode6XKK(vxIndex, expectedByte);
        uint16_t loadReg2SoundTimer = chip8::opcodeFX18(vxIndex);

        OpcodeList opcodes = {
            loadNumber10,        // LD Vx,0x10
            loadReg2SoundTimer,  // LD ST,Vx
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == expectedByte);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().st == expectedByte);
    }
}
