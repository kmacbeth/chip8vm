#include <catch2/catch.hpp>
#include <vector>

#include <memory.hpp>
#include <cpu.hpp>

using OpcodeList = std::vector<uint16_t>;

class Chip8Vm
{
        chip8::Memory memory_;
        chip8::Cpu    cpu_;

    public:
        Chip8Vm()
            : memory_()
            , cpu_(memory_)
        {
        }

        void loadCode(OpcodeList & opcodes)
        {
            memory_.storeOpcodes(opcodes.data(), opcodes.size());
        }

        void run()
        {
            cpu_.tick();
            cpu_.dumpRegisters();
        }

        uint8_t getMemoryByte(uint16_t address)
        {
            return memory_.loadData(address);
        }

        uint16_t getMemoryWord(uint16_t address)
        {
            uint16_t data = 0;

            data |= memory_.loadData(address);
            data |= (memory_.loadData(address+1) << 8);

            return data;
        }
};


TEST_CASE("Test CPU opcodes", "[cpu]")
{
    Chip8Vm vm;

    SECTION("Load number to Vx register")
    {
        OpcodeList opcodes = {
            0x6AAB   // LD VA,0xAB
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getMemoryByte(0xFFA) == 0xAB);
    }

    SECTION("Load Vy register to Vx register")
    {
        OpcodeList opcodes = {
            0x6AAB,  // LD VA,0xAB
            0x8CA0   // LD VC,VA
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getMemoryByte(0xFFA) == 0xAB);

        vm.run();
        REQUIRE(vm.getMemoryByte(0xFFC) == 0xAB);
    }

    SECTION("Load address to I register")
    {
        OpcodeList opcodes = {
            0xA123   // LD I,123
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getMemoryWord(0xFE2) == 0x123);
    }

    SECTION("Load Vx register to DT register")
    {
        OpcodeList opcodes = {
            0x6310,  // LD V3,0x10
            0xF315,  // LD DT,V3
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getMemoryByte(0xFF3) == 0x10);

        vm.run();
        REQUIRE(vm.getMemoryByte(0xFE5) == 0x10);
    }

    SECTION("Load DT register to Vx register")
    {
        OpcodeList opcodes = {
            0x6410,  // LD V4,0x10
            0xF407,  // LD DT,V4
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getMemoryByte(0xFF4) == 0x10);

        vm.run();
        REQUIRE(vm.getMemoryByte(0xFE4) == 0x00);
    }
}
