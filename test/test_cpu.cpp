#include <catch2/catch.hpp>
#include <vector>

#include <memory.hpp>
#include <cpu.hpp>

using OpcodeList = chip8::Memory::Words;

class Chip8Vm
{
    public:
        Chip8Vm()
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

uint16_t buildOpcodeLoadNumber(uint16_t vxIndex, uint16_t byte)
{
    return (0x6000 | ((vxIndex & 0xF) << 8) | (byte & 0xFF));
}


TEST_CASE("Test CPU opcodes", "[cpu][opcode]")
{
    Chip8Vm vm;

    // TODO: do parameterized test to test the 16 registers.
    SECTION("Load number to Vx register")
    {
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0xAB;

        uint16_t loadNumberAB = buildOpcodeLoadNumber(vxIndex, expectedByte);

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

        uint16_t loadNumberAB = buildOpcodeLoadNumber(vxIndex, expectedByte);
        uint16_t loadReg2Reg  = 0x8000 | (((vxIndex + 1) & 0xF) << 8) | ((vxIndex & 0xF) << 4);

        OpcodeList opcodes = {
            loadNumberAB,  // LD Vx,byte
            loadReg2Reg    // LD VC,VA
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == expectedByte);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[(vxIndex + 1) & 0xF] == expectedByte);
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
        auto vxIndex = GENERATE(Catch::Generators::range(0x0, 0x10));
        uint16_t expectedByte = 0x10;

        uint16_t loadNumber10 = buildOpcodeLoadNumber(vxIndex, expectedByte);
        uint16_t loadReg2DelayTimer = 0xF015 | ((vxIndex & 0xF) << 8);

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

        uint16_t loadNumber10 = buildOpcodeLoadNumber(vxIndex, expectedByte);
        uint16_t loadDelayTimer2Reg = 0xF007 | ((vxIndex & 0xF) << 8);

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

        uint16_t loadNumber10 = buildOpcodeLoadNumber(vxIndex, expectedByte);
        uint16_t loadReg2SoundTimer = 0xF018 | ((vxIndex & 0xF) << 8);

        OpcodeList opcodes = {
            loadNumber10,  // LD Vx,0x10
            loadReg2SoundTimer,  // LD ST,Vx
        };

        vm.loadCode(opcodes);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().vx[vxIndex] == expectedByte);

        vm.run();
        REQUIRE(vm.getCpuRegCtx().st == expectedByte);
    }
}
