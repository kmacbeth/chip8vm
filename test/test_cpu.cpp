#include <catch2/catch.hpp>
#include <vector>

#include <memory.hpp>
#include <cpu.hpp>

using OpcodeList = std::vector<uint16_t>;

class Chip8Vm
{
        chip8::Memory memory_;
        chip8::Cpu cpu_;
        chip8::Cpu::RegContext cpuRegCtx_;

    public:
        Chip8Vm()
            : memory_()
            , cpu_(memory_)
            , cpuRegCtx_()
        {
        }

        void loadCode(OpcodeList & opcodes)
        {
            memory_.storeOpcodes(opcodes.data(), opcodes.size());
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
        REQUIRE(vm.getCpuRegCtx().vx[0xA] == 0xAB);
    }

    SECTION("Load Vy register to Vx register")
    {
        OpcodeList opcodes = {
            0x6AAB,  // LD VA,0xAB
            0x8CA0   // LD VC,VA
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[0xA] == 0xAB);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[0xC] == 0xAB);
    }

    SECTION("Load address to I register")
    {
        OpcodeList opcodes = {
            0xA123   // LD I,123
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().i == 0x123);
    }

    SECTION("Load Vx register to DT register")
    {
        OpcodeList opcodes = {
            0x6310,  // LD V3,0x10
            0xF315,  // LD DT,V3
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[0x3] == 0x10);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().dt == 0x10);
    }

    SECTION("Load DT register to Vx register")
    {
        OpcodeList opcodes = {
            0x6410,  // LD V4,0x10
            0xF407,  // LD DT,V4
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[0x4] == 0x10);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[0x4] == 0x00);
    }
}
