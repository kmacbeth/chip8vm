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

#include "fake_gpu.hpp"
#include "fake_keyboard.hpp"

using OpcodeList = chip8::Memory::Words;
using Data = chip8::Memory::Bytes;

class Chip8TestVm
{
    public:
        Chip8TestVm()
            : memory_{chip8::SYSTEM_MEMORY_SIZE}
            , framebuffer_{chip8::FRAMEBUFFER_SIZE}
            , fakeGpu_{std::make_shared<chip8::FakeGpu>()}
            , fakeKeyboard_{std::make_shared<chip8::FakeKeyboard>()}
            , cpu_{std::make_shared<chip8::CpuImpl>(memory_, fakeGpu_, fakeKeyboard_)}
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

        chip8::Debugger const& debugger() { return debugger_; }

        chip8::FakeGpu & gpu() { return *fakeGpu_.get(); }
        chip8::FakeKeyboard & keyboard() { return *fakeKeyboard_.get(); }

    private:
        chip8::Memory memory_;
        chip8::Memory framebuffer_;
        std::shared_ptr<chip8::FakeGpu> fakeGpu_;
        std::shared_ptr<chip8::FakeKeyboard> fakeKeyboard_;
        std::shared_ptr<chip8::Cpu> cpu_;
        chip8::Debugger debugger_;
};


TEST_CASE("Test clear display", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto opcodes = OpcodeList {
        chip8::opcode::encode00E0()
    };

    vm.storeCode(opcodes);

    vm.run();
    REQUIRE(vm.gpu().clearCount == 1);
}

TEST_CASE("Test jump to address", "[opcode]")
{
    auto vm = Chip8TestVm{};

    uint16_t address = chip8::Cpu::PROGRAM_START + 0x0008;

    auto opcodes = OpcodeList {
        chip8::opcode::encode1NNN(address)
    };

    vm.storeCode(opcodes);

    vm.run();
    REQUIRE(vm.debugger().getProgramCounter() == address);
    REQUIRE(vm.debugger().getStackPointer() == 0);
}

TEST_CASE("Test call subroutine", "[opcode]")
{
    auto vm = Chip8TestVm{};

    uint16_t address = chip8::Cpu::PROGRAM_START + 0x0008;

    auto opcodes = OpcodeList {
        chip8::opcode::encode2NNN(address)
    };

    vm.storeCode(opcodes);

    vm.run();
    REQUIRE(vm.debugger().getStackPointer() == 1);
    REQUIRE(vm.debugger().getProgramCounter() == address);
}

TEST_CASE("Test return from subroutine", "[opcode]")
{
    auto vm = Chip8TestVm{};

    uint16_t address = chip8::Cpu::PROGRAM_START + 0x0008;

    auto opcodes = OpcodeList {
        chip8::opcode::encode2NNN(address),
        0x0000, 0x0000, 0x0000,
        chip8::opcode::encode00EE()
    };

    vm.storeCode(opcodes);

    vm.run();
    REQUIRE(vm.debugger().getStackPointer() == 1);
    REQUIRE(vm.debugger().getProgramCounter() == address);

    vm.run();
    REQUIRE(vm.debugger().getStackPointer() == 0);
    REQUIRE(vm.debugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 2);
}

TEST_CASE("Test load number to Vx register", "[opcode]")
{
    auto vm = Chip8TestVm{};
    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));

    uint16_t expectedByte = 0xAB;

    auto opcodes = OpcodeList {
        chip8::opcode::encode6XKK(vxIndex, expectedByte)
    };

    vm.storeCode(opcodes);

    vm.run();
    REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte);
}

TEST_CASE("Test skip next opcode if Vx register is equal to number", "[opcode]")
{
    auto vm = Chip8TestVm{};
    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));

    SECTION("Vx register is equal to number")
    {
        auto opcodes = OpcodeList {
            chip8::opcode::encode3XKK(vxIndex, 0x00),
        };

        vm.storeCode(opcodes);

        CHECK(vm.debugger().getRegisterVx(vxIndex) == 0x00);

        vm.run();
        REQUIRE(vm.debugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 4);
    }

    SECTION("Vx register is not equal to number")
    {
        auto opcodes = OpcodeList {
            chip8::opcode::encode3XKK(vxIndex, 0xFF)
        };

        vm.storeCode(opcodes);

        CHECK(vm.debugger().getRegisterVx(vxIndex) == 0x00);

        vm.run();
        REQUIRE(vm.debugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 2);
    }
}

TEST_CASE("Test skip next opcode if Vx register is not equals to number", "[opcode]")
{
    auto vm = Chip8TestVm{};
    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));

    SECTION("Vx register is not equal to number")
    {
        auto opcodes = OpcodeList {
            chip8::opcode::encode4XKK(vxIndex, 0xFF),
        };

        vm.storeCode(opcodes);

        CHECK(vm.debugger().getRegisterVx(vxIndex) == 0x00);

        vm.run();
        REQUIRE(vm.debugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 4);
    }

    SECTION("Vx register is equal to number")
    {
        auto opcodes = OpcodeList {
            chip8::opcode::encode4XKK(vxIndex, 0x00),
        };

        vm.storeCode(opcodes);

        CHECK(vm.debugger().getRegisterVx(vxIndex) == 0x00);

        vm.run();
        REQUIRE(vm.debugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 2);
    }
}

TEST_CASE("Test skip next opcode if Vx register equals Vy register", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
    auto vyIndex = (vxIndex + 1) & 0xF;

    SECTION("Vx register is equal Vy register")
    {
        auto opcodes = OpcodeList {
            chip8::opcode::encode5XY0(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        CHECK(vm.debugger().getRegisterVx(vxIndex) == 0x00);
        CHECK(vm.debugger().getRegisterVx(vyIndex) == 0x00);

        vm.run();
        REQUIRE(vm.debugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 4);
    }

    SECTION("Vx register is not equal to Vy register")
    {
        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, 0x01),
            chip8::opcode::encode5XY0(vxIndex, vyIndex)
        };

        vm.storeCode(opcodes);

        vm.run();

        CHECK(vm.debugger().getRegisterVx(vxIndex) == 0x01);
        CHECK(vm.debugger().getRegisterVx(vyIndex) == 0x00);

        vm.run();
        REQUIRE(vm.debugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 4);
    }
}

TEST_CASE("Test add number to Vx register", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
    uint16_t expectedByte = 0x10;

    auto opcodes = OpcodeList {
        chip8::opcode::encode7XKK(vxIndex, expectedByte),
    };

    vm.storeCode(opcodes);
    CHECK(vm.debugger().getRegisterVx(vxIndex) == 0);

    vm.run();
    REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte);
}

TEST_CASE("Test load Vy register to Vx register", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
    auto vyIndex = (vxIndex + 1) & 0xF;

    uint16_t expectedByte = 0xAB;

    auto opcodes = OpcodeList {
        chip8::opcode::encode6XKK(vxIndex, expectedByte),
        chip8::opcode::encode8XY0(vyIndex, vxIndex)
    };

    vm.storeCode(opcodes);

    vm.run();
    REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte);

    vm.run();
    REQUIRE(vm.debugger().getRegisterVx(vyIndex) == expectedByte);
}

TEST_CASE("Test OR register Vx with register Vy", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
    auto vyIndex = (vxIndex + 1) & 0xF;

    uint16_t expectedByte1 = 0xAA;
    uint16_t expectedByte2 = 0x5F;

    auto opcodes = OpcodeList {
        // Load bytes
        chip8::opcode::encode6XKK(vxIndex, expectedByte1),
        chip8::opcode::encode6XKK(vyIndex, expectedByte2),
        // OR bytes
        chip8::opcode::encode8XY1(vxIndex, vyIndex),
    };

    vm.storeCode(opcodes);

    vm.run();
    vm.run();
    REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte1);
    REQUIRE(vm.debugger().getRegisterVx(vyIndex) == expectedByte2);

    vm.run();
    REQUIRE(vm.debugger().getRegisterVx(vxIndex) == (expectedByte1 | expectedByte2));
}

TEST_CASE("Test AND register Vx with register Vy", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
    auto vyIndex = (vxIndex + 1) & 0xF;

    uint16_t expectedByte1 = 0xAA;
    uint16_t expectedByte2 = 0x3F;

    auto opcodes = OpcodeList {
        // Load bytes
        chip8::opcode::encode6XKK(vxIndex, expectedByte1),
        chip8::opcode::encode6XKK(vyIndex, expectedByte2),
        // OR bytes
        chip8::opcode::encode8XY2(vxIndex, vyIndex),
    };

    vm.storeCode(opcodes);

    vm.run();
    vm.run();
    REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte1);
    REQUIRE(vm.debugger().getRegisterVx(vyIndex) == expectedByte2);

    vm.run();
    REQUIRE(vm.debugger().getRegisterVx(vxIndex) == (expectedByte1 & expectedByte2));
}

TEST_CASE("Test XOR register Vx with register Vy", "[opcode]")
{
    auto vm = Chip8TestVm{};

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
    REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte1);
    REQUIRE(vm.debugger().getRegisterVx(vyIndex) == expectedByte2);

    vm.run();
    REQUIRE(vm.debugger().getRegisterVx(vxIndex) == (expectedByte1 ^ expectedByte2));

}

TEST_CASE("Test add register Vx with register Vy", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0xF));
    auto vyIndex = (vxIndex + 1) % 15;

    SECTION("Add without overflow")
    {
        uint16_t expectedByte1 = 0x22;
        uint16_t expectedByte2 = 0x33;

        auto opcodes = OpcodeList {
            // Load 2 bytes
            chip8::opcode::encode6XKK(vxIndex, expectedByte1),
            chip8::opcode::encode6XKK(vyIndex, expectedByte2),
            chip8::opcode::encode8XY4(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        vm.run();
        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte1);
        REQUIRE(vm.debugger().getRegisterVx(vyIndex) == expectedByte2);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == ((expectedByte1 + expectedByte2) & 0xFF));
        REQUIRE(vm.debugger().getRegisterVx(0xF) == 0x0);
    }

    SECTION("Add with overflow")
    {
        uint16_t expectedByte3 = 0xC2;
        uint16_t expectedByte4 = 0x53;

        auto opcodes = OpcodeList {
            // Load 2 bytes
            chip8::opcode::encode6XKK(vxIndex, expectedByte3),
            chip8::opcode::encode6XKK(vyIndex, expectedByte4),
            chip8::opcode::encode8XY4(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        vm.run();
        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte3);
        REQUIRE(vm.debugger().getRegisterVx(vyIndex) == expectedByte4);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == ((expectedByte3 + expectedByte4) & 0xFF));
        REQUIRE(vm.debugger().getRegisterVx(0xF) == 0x1);
    }
}

TEST_CASE("Test subtract register Vx with register Vy", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0xF));
    auto vyIndex = (vxIndex + 1) % 15;

    SECTION("Subtract without overflow")
    {
        uint16_t expectedByte1 = 0x33;
        uint16_t expectedByte2 = 0x22;

        auto opcodes = OpcodeList {
            // Normal add with no overflow
            chip8::opcode::encode6XKK(vxIndex, expectedByte1),
            chip8::opcode::encode6XKK(vyIndex, expectedByte2),
            chip8::opcode::encode8XY5(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        // No overflow
        vm.run();
        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte1);
        REQUIRE(vm.debugger().getRegisterVx(vyIndex) == expectedByte2);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == ((expectedByte1 - expectedByte2) & 0xFF));
        REQUIRE(vm.debugger().getRegisterVx(0xF) == 0x1);
    }

    SECTION("Subtract with overflow")
    {
        uint16_t expectedByte3 = 0x53;
        uint16_t expectedByte4 = 0x63;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, expectedByte3),
            chip8::opcode::encode6XKK(vyIndex, expectedByte4),
            chip8::opcode::encode8XY5(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        vm.run();
        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte3);
        REQUIRE(vm.debugger().getRegisterVx(vyIndex) == expectedByte4);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == ((expectedByte3 - expectedByte4) & 0xFF));
        REQUIRE(vm.debugger().getRegisterVx(0xF) == 0x0);
    }
}

TEST_CASE("Shift right register Vy to register Vx", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0xF));
    auto vyIndex = (vxIndex + 1) % 15;

    SECTION("Shift LSB doesn't set VF")
    {
        uint16_t expectedByte = 0xAA;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vyIndex, expectedByte),
            chip8::opcode::encode8XY6(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vyIndex) == expectedByte);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == (expectedByte >> 1));
        REQUIRE(vm.debugger().getRegisterVx(0xF) == (expectedByte & 0x1));
    }

    SECTION("Shift LSB sets VF")
    {
        uint16_t expectedByte = 0x55;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vyIndex, expectedByte),
            chip8::opcode::encode8XY6(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vyIndex) == expectedByte);

        vm.run();
        REQUIRE(static_cast<int>(vm.debugger().getRegisterVx(vxIndex)) == (expectedByte >> 1));
        REQUIRE(static_cast<int>(vm.debugger().getRegisterVx(0xF)) == (expectedByte & 0x1));
    }
}

TEST_CASE("Test subtract register Vy with register Vx", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0xF));
    auto vyIndex = (vxIndex + 1) % 15;

    SECTION("Subtract without overflow")
    {
        uint16_t expectedByte1 = 0x22;
        uint16_t expectedByte2 = 0x33;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, expectedByte1),
            chip8::opcode::encode6XKK(vyIndex, expectedByte2),
            chip8::opcode::encode8XY7(vxIndex, vyIndex)
        };

        vm.storeCode(opcodes);

        vm.run();
        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte1);
        REQUIRE(vm.debugger().getRegisterVx(vyIndex) == expectedByte2);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == ((expectedByte2 - expectedByte1) & 0xFF));
        REQUIRE(vm.debugger().getRegisterVx(0xF) == 0x1);
    }

    SECTION("Subtract with overflow")
    {
        uint16_t expectedByte3 = 0x63;
        uint16_t expectedByte4 = 0x53;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, expectedByte3),
            chip8::opcode::encode6XKK(vyIndex, expectedByte4),
            chip8::opcode::encode8XY7(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        vm.run();
        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte3);
        REQUIRE(vm.debugger().getRegisterVx(vyIndex) == expectedByte4);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == ((expectedByte4 - expectedByte3) & 0xFF));
        REQUIRE(vm.debugger().getRegisterVx(0xF) == 0x0);
    }
}

TEST_CASE("Test shift left register Vy to register Vx", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0xF));
    auto vyIndex = (vxIndex + 1) % 15;

    SECTION("Shift MSB doesn't set VF")
    {
        uint16_t expectedByte = 0xAA;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vyIndex, expectedByte),
            chip8::opcode::encode8XYE(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vyIndex) == expectedByte);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == ((expectedByte << 1) & 0xFF));
        REQUIRE(vm.debugger().getRegisterVx(0xF) == (expectedByte & 0x80));
    }

    SECTION("Shift MSB sets VF")
    {
        uint16_t expectedByte = 0xAA;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vyIndex, expectedByte),
            chip8::opcode::encode8XYE(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vyIndex) == expectedByte);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == ((expectedByte << 1) & 0xFF));
        REQUIRE(vm.debugger().getRegisterVx(0xF) == (expectedByte & 0x80));
    }
}

TEST_CASE("Skip next if register Vx not equals Vy", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
    auto vyIndex = (vxIndex + 1) & 0xF;


    SECTION("Register Vx is equal to Vy")
    {
        auto opcodes = OpcodeList {
            chip8::opcode::encode9XY0(vxIndex, vyIndex),
        };

        vm.storeCode(opcodes);
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == 0x00);

        vm.run();
        REQUIRE(vm.debugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 0x2);
    }

    SECTION("Register Vx is not equal to Vy")
    {
        uint16_t expectedByte = 0xAA;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, expectedByte),
            chip8::opcode::encode9XY0(vyIndex, vxIndex)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte);

        vm.run();
        REQUIRE(vm.debugger().getProgramCounter() == chip8::Cpu::PROGRAM_START + 0x6);
    }
}

TEST_CASE("Load address to I register", "[opcode]")
{
    auto vm = Chip8TestVm{};

    uint16_t expectedAddress = 0x123;

    auto opcodes = OpcodeList {
        chip8::opcode::encodeANNN(expectedAddress)
    };

    vm.storeCode(opcodes);

    vm.run();
    REQUIRE(vm.debugger().getRegisterI() == expectedAddress);
}

TEST_CASE("Jump to location plus V0 offset")
{
    auto vm = Chip8TestVm{};

    SECTION("Jump with zero offset")
    {
        uint16_t address1 = chip8::Cpu::PROGRAM_START + 0x4;

        auto opcodes = OpcodeList {
            chip8::opcode::encodeBNNN(address1),
        };

        vm.storeCode(opcodes);

        REQUIRE(vm.debugger().getRegisterVx(0) == 0);

        vm.run();
        REQUIRE(vm.debugger().getProgramCounter() == address1);
    }

    SECTION("Jump with offset")
    {
        uint16_t offset = 0x2;
        uint16_t address2 = chip8::Cpu::PROGRAM_START + 0xA;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(0, offset),
            chip8::opcode::encodeBNNN(address2)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(0) == offset);

        vm.run();
        REQUIRE(vm.debugger().getProgramCounter() == (address2 + offset));
    }
}

TEST_CASE("Generate random 8-bit number", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));

    SECTION("Generate with base-2 modulo mask")
    {
        uint16_t mask1 = 0x7F;

        auto opcodes = OpcodeList {
            chip8::opcode::encodeCXKK(vxIndex, mask1)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) < (mask1 + 1));
    }

    SECTION("Generate with non base-2 modulo mask")
    {
        uint16_t mask2 = 0x05;

        auto opcodes = OpcodeList {
            chip8::opcode::encodeCXKK(vxIndex, mask2)
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(vxIndex) < (mask2 - 4));
    }
}

TEST_CASE("Draw sprite", "[opcode]")
{
    auto vm = Chip8TestVm{};

    uint16_t START_DATA_ADDRESS = 0x800;

    auto x = 1u;
    auto y = 2u;

    SECTION("Draw sprite erases pixel")
    {
        auto sprite = chip8::Sprite{0x01};

        vm.storeData(START_DATA_ADDRESS, sprite);

        auto opcodes = OpcodeList {
            chip8::opcode::encodeANNN(START_DATA_ADDRESS),
            chip8::opcode::encodeDXYN(x, y, sprite.size())
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.debugger().getRegisterI() == START_DATA_ADDRESS);

        vm.gpu().spriteErased = true;

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(0xF) == 0x1);

        auto drawContext = vm.gpu().drawContext;
        REQUIRE(drawContext.x == x);
        REQUIRE(drawContext.y == y);
        REQUIRE_THAT(drawContext.sprite, Catch::Contains(sprite));
    }

    SECTION("Draw sprite does not erase pixel.")
    {
        auto sprite = chip8::Sprite{0x00};

        vm.storeData(START_DATA_ADDRESS, sprite);

        auto opcodes = OpcodeList {
            chip8::opcode::encodeANNN(START_DATA_ADDRESS),
            chip8::opcode::encodeDXYN(x, y, sprite.size())
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.debugger().getRegisterI() == START_DATA_ADDRESS);

        vm.gpu().spriteErased = false;

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(0xF) == 0x0);

        auto drawContext = vm.gpu().drawContext;
        REQUIRE(drawContext.x == x);
        REQUIRE(drawContext.y == y);
        REQUIRE_THAT(drawContext.sprite, Catch::Contains(sprite));
    }

    SECTION("Draw sprite multiple length")
    {
        size_t  spriteLength = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint8_t spriteByte = 0xA5;

        auto sprite = chip8::Sprite(spriteLength, spriteByte);

        vm.storeData(START_DATA_ADDRESS, sprite);

        auto opcodes = OpcodeList {
            chip8::opcode::encodeANNN(START_DATA_ADDRESS),
            chip8::opcode::encodeDXYN(x, y, sprite.size())
        };

        vm.storeCode(opcodes);

        vm.run();
        REQUIRE(vm.debugger().getRegisterI() == START_DATA_ADDRESS);

        vm.gpu().spriteErased = true;

        vm.run();
        REQUIRE(vm.debugger().getRegisterVx(0xF) == 0x1);

        auto drawContext = vm.gpu().drawContext;
        REQUIRE(drawContext.x == x);
        REQUIRE(drawContext.y == y);

        REQUIRE_THAT(drawContext.sprite, Catch::Contains(sprite));
    }
}

TEST_CASE("Skip next instruction if key pressed", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));

    SECTION("Key is not pressed")
    {
        uint8_t key = 0x9;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, key),
            chip8::opcode::encodeEX9E(vxIndex)
        };

        vm.storeCode(opcodes);

        vm.run();
        vm.run();
        REQUIRE(vm.debugger().getProgramCounter() == (chip8::Cpu::PROGRAM_START + 4));
    }

    SECTION("Key is pressed")
    {
        uint8_t key = 0x3;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, key),
            chip8::opcode::encodeEX9E(vxIndex)
        };

        vm.storeCode(opcodes);

        vm.keyboard().pressKey(key);

        vm.run();
        vm.run();
        REQUIRE(vm.debugger().getProgramCounter() == (chip8::Cpu::PROGRAM_START + 6));

        vm.keyboard().releaseKey(key);
    }
}

TEST_CASE("Skip next instruction if key is not pressed", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = 0;

    SECTION("Key is not pressed")
    {
        int key = 0x9;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, key),
            chip8::opcode::encodeEXA1(vxIndex)
        };

        vm.storeCode(opcodes);

        vm.run();
        vm.run();
        REQUIRE(vm.debugger().getProgramCounter() == (chip8::Cpu::PROGRAM_START + 6));
    }

    SECTION("Key is pressed")
    {
        int key = 0x3;

        auto opcodes = OpcodeList {
            chip8::opcode::encode6XKK(vxIndex, key),
            chip8::opcode::encodeEXA1(vxIndex)
        };

        vm.storeCode(opcodes);

        vm.keyboard().pressKey(key);

        vm.run();
        vm.run();
        REQUIRE(vm.debugger().getProgramCounter() == (chip8::Cpu::PROGRAM_START + 4));

        vm.keyboard().releaseKey(key);
    }
}

TEST_CASE("Load DT register to Vx register", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
    uint16_t expectedByte = 0x10;

    auto opcodes = OpcodeList {
        chip8::opcode::encode6XKK(vxIndex, expectedByte),
        chip8::opcode::encodeFX07(vxIndex)
    };

    vm.storeCode(opcodes);

    vm.run();
    REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte);

    vm.run();
    REQUIRE(vm.debugger().getRegisterVx(vxIndex) == 0x00);
}

TEST_CASE("Wait and load pressed key in Vx register")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
    uint16_t expectedByte = 0x10;

    auto opcodes = OpcodeList {
        chip8::opcode::encode6XKK(vxIndex, expectedByte),
        chip8::opcode::encodeFX0A(vxIndex)
    };
}

TEST_CASE("Load Vx register to DT register", "[opcode]")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
    uint16_t expectedByte = 0x10;

    auto opcodes = OpcodeList {
        chip8::opcode::encode6XKK(vxIndex, expectedByte),
        chip8::opcode::encodeFX15(vxIndex)
    };

    vm.storeCode(opcodes);

    vm.run();
    REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte);

    vm.run();
    REQUIRE(vm.debugger().getDelayTimer() == expectedByte);
}

TEST_CASE("Load Vx to ST register")
{
    auto vm = Chip8TestVm{};

    auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
    uint16_t expectedByte = 0x10;

    auto opcodes = OpcodeList {
        chip8::opcode::encode6XKK(vxIndex, expectedByte),
        chip8::opcode::encodeFX18(vxIndex)
    };

    vm.storeCode(opcodes);

    vm.run();
    REQUIRE(vm.debugger().getRegisterVx(vxIndex) == expectedByte);

    vm.run();
    REQUIRE(vm.debugger().getSoundTimer() == expectedByte);
}
