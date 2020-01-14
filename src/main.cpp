/*
 * MIT License
 *
 * Copyright (c) 2019 Martin Lafreniere
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
#include <memory.hpp>
#include <cpu.hpp>

void runCpuTicks(chip8::Memory & memory, chip8::Cpu & cpu, uint16_t * code, uint16_t count)
{
    memory.storeOpcodes(code, count);

    cpu.reset();
    for (uint32_t index = 0; index < count; ++index)
    {
        cpu.tick();
        cpu.printTrace();
    }
}

void testLoadNumber(chip8::Memory & memory, chip8::Cpu & cpu)
{
    uint16_t code[] = {
        0x6AAB   // LD VA,0xAB
    };
    runCpuTicks(memory, cpu, code, sizeof(code) / 2);
}

void testLoadRegister(chip8::Memory & memory, chip8::Cpu & cpu)
{
    uint16_t code[] = {
        0x6AAB,  // LD VA,0xAB
        0x8CA0   // LD VC,VA
    };
    runCpuTicks(memory, cpu, code, sizeof(code) / 2);
}

void testLoadIRegister(chip8::Memory & memory, chip8::Cpu & cpu)
{
    uint16_t code[] = {
        0xA123   // LD I,123
    };
    runCpuTicks(memory, cpu, code, sizeof(code) / 2);
}

void testLoadDelayTimer(chip8::Memory & memory, chip8::Cpu & cpu)
{
    uint16_t code[] = {
        0x6310,  // LD V3,0x10
        0xF315,  // LD DT,V3
        0xF407,  // LD V4,DT
    };
    runCpuTicks(memory, cpu, code, sizeof(code) / 2);
}

int main(int argc, char * argv[])
{
    chip8::Memory memory;
    chip8::Cpu cpu(memory);

    testLoadNumber(memory, cpu);
    testLoadRegister(memory, cpu);
    testLoadIRegister(memory, cpu);
    testLoadDelayTimer(memory, cpu);

    return 0;
}
