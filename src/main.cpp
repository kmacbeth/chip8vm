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
#include <gpu.hpp>
#include <keyboard.hpp>
#include <cpu.hpp>

int main(int argc, char * argv[])
{
    std::shared_ptr<chip8::Memory> memory = std::make_shared<chip8::Memory>(chip8::SYSTEM_MEMORY_SIZE);
    chip8::Memory framebuffer = chip8::Memory{chip8::FRAMEBUFFER_SIZE};

    std::shared_ptr<chip8::Gpu> gpu = std::make_shared<chip8::GpuImpl>(framebuffer);
    std::shared_ptr<chip8::Keyboard> keyboard = std::make_shared<chip8::KeyboardImpl>();
    std::shared_ptr<chip8::Cpu> cpu = std::make_shared<chip8::CpuImpl>(memory, keyboard, gpu);

    return 0;
}
