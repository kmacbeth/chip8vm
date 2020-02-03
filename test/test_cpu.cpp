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
#include <debugger.hpp>
#include <fake_gpu.hpp>

using OpcodeList = chip8::Memory::Words;
using Data = chip8::Memory::Bytes;

class Chip8TestVm
{
    public:
        Chip8TestVm()
            : memory_{chip8::SYSTEM_MEMORY_SIZE}
            , framebuffer_{chip8::FRAMEBUFFER_SIZE}
            , fakeGpu_{std::make_shared<chip8::FakeGpu>()}
            , cpu_{std::make_shared<chip8::CpuImpl>(memory_, fakeGpu_)}
            , debugger_{cpu_, memory_}
        {
        }

        void storeCode(OpcodeList const& opcodes)
        {
            memory_.storeBuffer(chip8::Cpu::PROGRAM_START, opcodes, chip8::Memory::Endian::LITTLE);
        }

        void storeData(uint16_t startAddress, Data const& data)
        {
            memory_.storeBuffer(startAddress, data);
        }

        void storeFramebuffer(chip8::Memory::Bytes const& bytes)
        {
            framebuffer_.storeBuffer(0x0000, bytes);
        }

        void loadFramebuffer(chip8::Memory::Bytes & bytes)
        {
            loadFramebufferData(0x0000, bytes);
        }

        void loadFramebufferData(uint16_t startAddress, chip8::Memory::Bytes & bytes)
        {
            for (size_t offset = 0; offset < bytes.size(); ++offset)
            {
                uint16_t address = startAddress + offset;

                bytes[offset] = framebuffer_.load<uint8_t>(address);
            }
        }

        void run()
        {
            debugger_.tick();
        }

        void setDebugTrace(chip8::Debugger::Traces traces)
        {
            debugger_.setTraces(traces);
        }

        chip8::Debugger const& getDebugger() { return debugger_; }

        chip8::FakeGpu & getFakeGpu() { return *fakeGpu_.get(); }

    private:
        chip8::Memory memory_;
        chip8::Memory framebuffer_;
        std::shared_ptr<chip8::FakeGpu> fakeGpu_;
        std::shared_ptr<chip8::Cpu> cpu_;
        chip8::Debugger debugger_;
};


TEST_CASE("Test CPU opcodes", "[cpu][opcode]")
{
    auto vm = Chip8TestVm{};

    SECTION("Clear display")
    {
        auto opcodes = OpcodeList {
            chip8::opcode::encode00E0()
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getFakeGpu().clearCount == 1);
    }

    SECTION("Jump to location")
    {
        uint16_t address = chip8::Cpu::PROGRAM_START + 0x0008;

        auto opcodes = OpcodeList {
            chip8::opcode::encode1NNN(address)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getDebugger().getProgramCounter() == address);
        REQUIRE(vm.getDebugger().getStackPointer() == 0);
    }

    SECTION("Call subroutine")
    {
        uint16_t address = chip8::Cpu::PROGRAM_START + 0x0008;

        auto opcodes = OpcodeList {
            chip8::opcode::encode2NNN(address)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getDebugger().getStackPointer() == 1);
        REQUIRE(vm.getDebugger().getProgramCounter() == address);
    }

    SECTION("Return from subroutine")
    {
        uint16_t address = chip8::Cpu::PROGRAM_START + 0x0008;

        auto opcodes = OpcodeList {
            chip8::opcode::encode2NNN(address),
            0x0000,
            0x0000,
            0x0000,
            chip8::opcode::encode00EE()
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getDebugger().getStackPointer() == 1);
        REQUIRE(vm.getDebugger().getProgramCounter() == address);

        vm.run();
        REQUIRE(vm.getDebugger().getStackPointer() == 0);
        REQUIRE(vm.getDebugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 2);
    }

    SECTION("Load number to Vx register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0xAB;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, expectedByte)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte);
    }

    SECTION("Skip next opcode if Vx register equals byte")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));

        auto opcodes = OpcodeList {
            chip8::opcode::encode3XKK(vxIndex, 0x00),
            0x0000,
            chip8::opcode::encode3XKK(vxIndex, 0xFF)
        };

        vm.storeCode(opcodes);
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == 0x00);

        vm.run();
        REQUIRE(vm.getDebugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 4);

        vm.run();
        REQUIRE(vm.getDebugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 6);
    }

    SECTION("Skip next opcode if Vx register not equals byte")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));

        auto opcodes = OpcodeList {
            chip8::opcode::encode4XKK(vxIndex, 0xFF),
            0x0000,
            chip8::opcode::encode4XKK(vxIndex, 0x00)
        };

        vm.storeCode(opcodes);
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == 0x00);

        vm.run();
        REQUIRE(vm.getDebugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 4);

        vm.run();
        REQUIRE(vm.getDebugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 6);
    }

    SECTION("Skip next opcode if Vx register equals Vy register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        auto vyIndex = (vxIndex + 1) & 0xF;

        auto opcodes = OpcodeList {
            chip8::opcode::encode5XY0(vxIndex, vyIndex),
            0x0000,
            chip8::opcode::encode6XKK(vxIndex, 0x01),
            chip8::opcode::encode5XY0(vxIndex, vyIndex)
        };

        vm.storeCode(opcodes);
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == 0x00);
        REQUIRE(vm.getDebugger().getRegisterVx(vyIndex) == 0x00);

        vm.run();
        REQUIRE(vm.getDebugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 4);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == 0x01);

        vm.run();
        REQUIRE(vm.getDebugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 8);
    }

    SECTION("Add byte to Vx register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0x10;

        auto opcodes = OpcodeList {
            chip8::opcode::encode7XKK(vxIndex, expectedByte),
            chip8::opcode::encode7XKK(vxIndex, expectedByte)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == (2 * expectedByte));
    }

    SECTION("Load Vy register to Vx register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        auto vyIndex = (vxIndex + 1) & 0xF;

        uint16_t expectedByte = 0xAB;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, expectedByte),
            chip8::opcode::encode8XY0(vyIndex, vxIndex)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vyIndex) == expectedByte);
    }

    SECTION("OR register Vx with register Vy")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        auto vyIndex = (vxIndex + 1) & 0xF;

        uint16_t expectedByte1 = 0xAA;
        uint16_t expectedByte2 = 0x5F;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, expectedByte1),
            chip8::opcode::encode6XKK(vyIndex, expectedByte2),
            chip8::opcode::encode8XY1(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        vm.run();
        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte1);
        REQUIRE(vm.getDebugger().getRegisterVx(vyIndex) == expectedByte2);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == (expectedByte1 | expectedByte2));

    }

    SECTION("AND register Vx with register Vy")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        auto vyIndex = (vxIndex + 1) & 0xF;

        uint16_t expectedByte1 = 0xAA;
        uint16_t expectedByte2 = 0x3F;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, expectedByte1),
            chip8::opcode::encode6XKK(vyIndex, expectedByte2),
            chip8::opcode::encode8XY2(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        vm.run();
        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte1);
        REQUIRE(vm.getDebugger().getRegisterVx(vyIndex) == expectedByte2);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == (expectedByte1 & expectedByte2));

    }

    SECTION("XOR register Vx with register Vy")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        auto vyIndex = (vxIndex + 1) & 0xF;

        uint16_t expectedByte1 = 0xAA;
        uint16_t expectedByte2 = 0x5F;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, expectedByte1),
            chip8::opcode::encode6XKK(vyIndex, expectedByte2),
            chip8::opcode::encode8XY3(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        vm.run();
        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte1);
        REQUIRE(vm.getDebugger().getRegisterVx(vyIndex) == expectedByte2);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == (expectedByte1 ^ expectedByte2));

    }

    SECTION("Add register Vx with register Vy")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0xF));
        auto vyIndex = (vxIndex + 1) % 15;

        uint16_t expectedByte1 = 0x22;
        uint16_t expectedByte2 = 0x33;
        uint16_t expectedByte3 = 0xC2;
        uint16_t expectedByte4 = 0x53;

        auto opcodes = OpcodeList {
            // Normal add with no overflow
            chip8::opcode::encode6XKK(vxIndex, expectedByte1),
            chip8::opcode::encode6XKK(vyIndex, expectedByte2),
            chip8::opcode::encode8XY4(vxIndex, vyIndex),
            // Add with overflow
            chip8::opcode::encode6XKK(vxIndex, expectedByte3),
            chip8::opcode::encode6XKK(vyIndex, expectedByte4),
            chip8::opcode::encode8XY4(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        // No overflow
        vm.run();
        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte1);
        REQUIRE(vm.getDebugger().getRegisterVx(vyIndex) == expectedByte2);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == ((expectedByte1 + expectedByte2) & 0xFF));
        REQUIRE(vm.getDebugger().getRegisterVx(0xF) == 0x0);

        // With overflow
        vm.run();
        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte3);
        REQUIRE(vm.getDebugger().getRegisterVx(vyIndex) == expectedByte4);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == ((expectedByte3 + expectedByte4) & 0xFF));
        REQUIRE(vm.getDebugger().getRegisterVx(0xF) == 0x1);
    }

    SECTION("Subtract register Vx with register Vy")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0xF));
        auto vyIndex = (vxIndex + 1) % 15;

        uint16_t expectedByte1 = 0x33;
        uint16_t expectedByte2 = 0x22;
        uint16_t expectedByte3 = 0x53;
        uint16_t expectedByte4 = 0x63;

        auto opcodes = OpcodeList {
            // Normal add with no overflow
            chip8::opcode::encode6XKK(vxIndex, expectedByte1),
            chip8::opcode::encode6XKK(vyIndex, expectedByte2),
            chip8::opcode::encode8XY5(vxIndex, vyIndex),
            // Add with overflow
            chip8::opcode::encode6XKK(vxIndex, expectedByte3),
            chip8::opcode::encode6XKK(vyIndex, expectedByte4),
            chip8::opcode::encode8XY5(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        // No overflow
        vm.run();
        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte1);
        REQUIRE(vm.getDebugger().getRegisterVx(vyIndex) == expectedByte2);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == ((expectedByte1 - expectedByte2) & 0xFF));
        REQUIRE(vm.getDebugger().getRegisterVx(0xF) == 0x1);

        // With overflow
        vm.run();
        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte3);
        REQUIRE(vm.getDebugger().getRegisterVx(vyIndex) == expectedByte4);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == ((expectedByte3 - expectedByte4) & 0xFF));
        REQUIRE(vm.getDebugger().getRegisterVx(0xF) == 0x0);
    }

    SECTION("Shift right register Vy to register Vx")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0xF));
        auto vyIndex = (vxIndex + 1) % 15;

        uint16_t expectedByte = 0xAA;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, expectedByte),
            chip8::opcode::encode8XY6(vyIndex, vxIndex),
            chip8::opcode::encode8XY6(vxIndex, vyIndex)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vyIndex) == (expectedByte >> 1));
        REQUIRE(vm.getDebugger().getRegisterVx(0xF) == (expectedByte & 0x1));

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == (expectedByte >> 2));
        REQUIRE(vm.getDebugger().getRegisterVx(0xF) == ((expectedByte >> 1) & 0x1));
    }

    SECTION("Subtract register Vy with register Vx")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0xF));
        auto vyIndex = (vxIndex + 1) % 15;

        uint16_t expectedByte1 = 0x22;
        uint16_t expectedByte2 = 0x33;
        uint16_t expectedByte3 = 0x63;
        uint16_t expectedByte4 = 0x53;

        auto opcodes = OpcodeList {
            // Normal add with no overflow
            chip8::opcode::encode6XKK(vxIndex, expectedByte1),
            chip8::opcode::encode6XKK(vyIndex, expectedByte2),
            chip8::opcode::encode8XY7(vxIndex, vyIndex),
            // Add with overflow
            chip8::opcode::encode6XKK(vxIndex, expectedByte3),
            chip8::opcode::encode6XKK(vyIndex, expectedByte4),
            chip8::opcode::encode8XY7(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        // No overflow
        vm.run();
        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte1);
        REQUIRE(vm.getDebugger().getRegisterVx(vyIndex) == expectedByte2);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == ((expectedByte2 - expectedByte1) & 0xFF));
        REQUIRE(vm.getDebugger().getRegisterVx(0xF) == 0x1);

        // With overflow
        vm.run();
        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte3);
        REQUIRE(vm.getDebugger().getRegisterVx(vyIndex) == expectedByte4);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == ((expectedByte4 - expectedByte3) & 0xFF));
        REQUIRE(vm.getDebugger().getRegisterVx(0xF) == 0x0);
    }

    SECTION("Shift left register Vy to register Vx")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0xF));
        auto vyIndex = (vxIndex + 1) % 15;

        uint16_t expectedByte = 0xAA;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, expectedByte),
            chip8::opcode::encode8XYE(vyIndex, vxIndex),
            chip8::opcode::encode8XYE(vxIndex, vyIndex)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vyIndex) == ((expectedByte << 1) & 0xFF));
        REQUIRE(vm.getDebugger().getRegisterVx(0xF) == (expectedByte & 0x80));

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == ((expectedByte << 2) & 0xFF));
        REQUIRE(vm.getDebugger().getRegisterVx(0xF) == ((expectedByte << 1) & 0x80));
    }

    SECTION("Skip next if register Vx not equals Vy")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        auto vyIndex = (vxIndex + 1) & 0xF;

        uint16_t expectedByte = 0xAA;

        auto opcodes = OpcodeList {
            chip8::opcode::encode9XY0(vxIndex, vyIndex),
            chip8::opcode::encode6XKK(vxIndex, expectedByte),
            chip8::opcode::encode9XY0(vyIndex, vxIndex)
        };

        vm.storeCode(opcodes);

        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == 0x00);

        vm.run();
        REQUIRE(vm.getDebugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 0x2);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte);

        vm.run();
        REQUIRE(vm.getDebugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 0x8);
    }

    SECTION("Load address to I register")
    {
        auto opcodes = OpcodeList {
            chip8::opcode::encodeANNN(0x123)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterI() == 0x123);
    }

    SECTION("Jump to location plus V0 offset")
    {
        uint16_t address1 = chip8::Cpu::PROGRAM_START + 0x4;
        uint16_t address2 = chip8::Cpu::PROGRAM_START + 0xA;
        uint16_t offset = 0x2;

        auto opcodes = OpcodeList {
            chip8::opcode::encodeBNNN(address1),
            0x0000,
            chip8::opcode::encode6XKK(0, offset),
            chip8::opcode::encodeBNNN(address2)
        };

        vm.storeCode(opcodes);

        REQUIRE(vm.getDebugger().getRegisterVx(0) == 0);

        vm.run();
        REQUIRE(vm.getDebugger().getProgramCounter() == address1);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(0) == offset);

        vm.run();
        REQUIRE(vm.getDebugger().getProgramCounter() == (address2 + offset));
    }

    SECTION("Generate random 8-bit number")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t mask1 = 0x7F;
        uint16_t mask2 = 0x05;

        auto opcodes = OpcodeList {
            chip8::opcode::encodeCXKK(vxIndex, mask1),
            chip8::opcode::encodeCXKK(vxIndex, mask2)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) < (mask1 + 1));

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) < (mask2 - 4));
    }

    SECTION("Draw sprite erase pixel.")
    {
        const uint16_t START_DATA_ADDRESS = 0x800;

        auto sprite = chip8::Sprite{0x01};
        auto x = 1u;
        auto y = 2u;

        vm.storeData(START_DATA_ADDRESS, sprite);

        auto opcodes = OpcodeList {
            chip8::opcode::encodeANNN(START_DATA_ADDRESS),
            chip8::opcode::encodeDXYN(x, y, sprite.size())
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterI() == START_DATA_ADDRESS);

        vm.getFakeGpu().spriteErased = true;

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(0xF) == 0x1);

        auto drawContext = vm.getFakeGpu().drawContext;
        REQUIRE(drawContext.x == x);
        REQUIRE(drawContext.y == y);
        REQUIRE(drawContext.sprite[0] == 0x1);
    }

    SECTION("Draw sprite does not erase pixel.")
    {
        const uint16_t START_DATA_ADDRESS = 0x800;

        auto sprite = chip8::Sprite{0x00};
        auto x = 1u;
        auto y = 2u;

        vm.storeData(START_DATA_ADDRESS, sprite);

        auto opcodes = OpcodeList {
            chip8::opcode::encodeANNN(START_DATA_ADDRESS),
            chip8::opcode::encodeDXYN(x, y, sprite.size())
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterI() == START_DATA_ADDRESS);

        vm.getFakeGpu().spriteErased = false;

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(0xF) == 0x0);

        auto drawContext = vm.getFakeGpu().drawContext;
        REQUIRE(drawContext.x == x);
        REQUIRE(drawContext.y == y);
        REQUIRE(drawContext.sprite[0] == 0x0);
    }

    SECTION("Load DT register to Vx register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0x10;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, expectedByte),
            chip8::opcode::encodeFX07(vxIndex)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == 0x00);
    }

    SECTION("Load Vx register to DT register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0x10;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, expectedByte),
            chip8::opcode::encodeFX15(vxIndex)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte);

        vm.run();
        REQUIRE(vm.getDebugger().getDelayTimer() == expectedByte);
    }


    SECTION("Load Vx to ST register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0x10;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, expectedByte),
            chip8::opcode::encodeFX18(vxIndex)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.getDebugger().getRegisterVx(vxIndex) == expectedByte);

        vm.run();
        REQUIRE(vm.getDebugger().getSoundTimer() == expectedByte);
    }
}
