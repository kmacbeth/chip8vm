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
#include <gpu.hpp>

using OpcodeList = chip8::Memory::Words;

class Chip8TestVm
{
    public:
        Chip8TestVm()
            : memory_{chip8::SYSTEM_MEMORY_SIZE}
            , frameBuffer_{chip8::FRAMEBUFFER_SIZE}
            , gpu_{frameBuffer_}
            , cpu_{memory_, gpu_}
            , cpuRegCtx_{}
        {
        }

        void storeCode(OpcodeList & opcodes)
        {
            memory_.storeBuffer(chip8::SYSTEM_START_POINT, opcodes, chip8::Memory::Endian::LITTLE);
        }


        void storeFrameBuffer(chip8::Memory::Bytes const& bytes)
        {
            frameBuffer_.storeBuffer(0x0000, bytes);
        }

        void loadFrameBuffer(chip8::Memory::Bytes & bytes)
        {
            for (size_t address = 0; address < frameBuffer_.getSize(); ++address)
            {
                bytes[address] = frameBuffer_.load<uint8_t>(address);
            }
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
        chip8::Memory frameBuffer_;
        chip8::Gpu gpu_;
        chip8::Cpu cpu_;
        chip8::Cpu::RegContext cpuRegCtx_;

};


TEST_CASE("Test CPU opcodes", "[cpu][opcode]")
{
    Chip8TestVm vm = Chip8TestVm{};

    SECTION("Clear display")
    {
        chip8::Memory::Bytes frameBuffer = chip8::Memory::Bytes(chip8::FRAMEBUFFER_SIZE, 0xFF);
        chip8::Memory::Bytes clearBuffer = chip8::Memory::Bytes(chip8::FRAMEBUFFER_SIZE, 0x00);

        vm.storeFrameBuffer(frameBuffer);

        OpcodeList opcodes = {
            chip8::opcode::encode00E0()
        };

        vm.storeCode(opcodes);

        vm.run();
        vm.loadFrameBuffer(frameBuffer);

        REQUIRE_THAT(frameBuffer, Catch::Equals(clearBuffer));
    }

    SECTION("Jump to location")
    {
        uint16_t address = chip8::SYSTEM_START_POINT + 0x0008;

        OpcodeList opcodes = {
            chip8::opcode::encode1NNN(address)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().pc == address);
        REQUIRE(vm.getCpuRegCtx().sp == 0);
    }

    SECTION("Call subroutine")
    {
        uint16_t address = chip8::SYSTEM_START_POINT + 0x0008;

        OpcodeList opcodes = {
            chip8::opcode::encode2NNN(address)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().sp == 1);
        REQUIRE(vm.getCpuRegCtx().pc == address);
    }

    SECTION("Return from subroutine")
    {
        uint16_t address = chip8::SYSTEM_START_POINT + 0x0008;

        OpcodeList opcodes = {
            chip8::opcode::encode2NNN(address),
            0x0000,
            0x0000,
            0x0000,
            chip8::opcode::encode00EE()
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().sp == 1);
        REQUIRE(vm.getCpuRegCtx().pc == address);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().sp == 0);
        REQUIRE(vm.getCpuRegCtx().pc == chip8::SYSTEM_START_POINT + 2);
    }

    SECTION("Load number to Vx register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0xAB;

        OpcodeList opcodes = {
            chip8::opcode::encode6XKK(vxIndex, expectedByte)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == expectedByte);
    }

    SECTION("Skip next opcode if Vx register equals byte")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));

        OpcodeList opcodes = {
            chip8::opcode::encode3XKK(vxIndex, 0x00),
            0x0000,
            chip8::opcode::encode3XKK(vxIndex, 0xFF)
        };

        vm.storeCode(opcodes);
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == 0x00);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().pc == chip8::SYSTEM_START_POINT + 4);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().pc == chip8::SYSTEM_START_POINT + 6);
    }

    SECTION("Skip next opcode if Vx register not equals byte")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));

        OpcodeList opcodes = {
            chip8::opcode::encode4XKK(vxIndex, 0xFF),
            0x0000,
            chip8::opcode::encode4XKK(vxIndex, 0x00)
        };

        vm.storeCode(opcodes);
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == 0x00);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().pc == chip8::SYSTEM_START_POINT + 4);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().pc == chip8::SYSTEM_START_POINT + 6);
    }

    SECTION("Skip next opcode if Vx register equals Vy register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));

        OpcodeList opcodes = {
            chip8::opcode::encode5XY0(vxIndex, (vxIndex + 1) & 0xF),
            0x0000,
            chip8::opcode::encode6XKK(vxIndex, 0x01),
            chip8::opcode::encode5XY0(vxIndex, (vxIndex + 1) & 0xF)
        };

        vm.storeCode(opcodes);
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == 0x00);
        REQUIRE(vm.getCpuRegCtx().vx[(vxIndex + 1) & 0xF] == 0x00);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().pc == chip8::SYSTEM_START_POINT + 4);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == 0x01);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().pc == chip8::SYSTEM_START_POINT + 8);
    }

    SECTION("Load Vy register to Vx register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0xAB;

        OpcodeList opcodes = {
            chip8::opcode::encode6XKK(vxIndex, expectedByte),
            chip8::opcode::encode8XY0((vxIndex + 1) & 0xF, vxIndex)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == expectedByte);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[(vxIndex + 1) & 0xF] == expectedByte);
    }

    SECTION("Load address to I register")
    {
        OpcodeList opcodes = {
            chip8::opcode::encodeANNN(0x123)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().i == 0x123);
    }

    SECTION("Load DT register to Vx register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0x10;


        OpcodeList opcodes = {
            chip8::opcode::encode6XKK(vxIndex, expectedByte),
            chip8::opcode::encodeFX07(vxIndex)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == expectedByte);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == 0x00);
    }

    SECTION("Load Vx register to DT register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0x10;

        OpcodeList opcodes = {
            chip8::opcode::encode6XKK(vxIndex, expectedByte),
            chip8::opcode::encodeFX15(vxIndex)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == expectedByte);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().dt == expectedByte);
    }


    SECTION("Load Vx to ST register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0x10;

        OpcodeList opcodes = {
            chip8::opcode::encode6XKK(vxIndex, expectedByte),
            chip8::opcode::encodeFX18(vxIndex)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == expectedByte);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().st == expectedByte);
    }
}
