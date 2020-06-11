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
#ifndef CHIP8_VIRTUALMACHINE_HPP
#define CHIP8_VIRTUALMACHINE_HPP

#include <SDL2/SDL.h>

#include <memory.hpp>
#include <gpu.hpp>
#include <keyboard.hpp>
#include <cpu.hpp>


namespace chip8 {

/// @brief CHIP-8 virtual machine.
class VirtualMachine
{
    public:
        VirtualMachine();
        ~VirtualMachine();

        bool initialize();
        void start();

    private:
        SDL_Window * window_;

        std::shared_ptr<chip8::Gpu> gpu_;
        std::shared_ptr<chip8::Keyboard> keyboard_;
        std::shared_ptr<chip8::Memory> memory_;
        std::shared_ptr<chip8::Cpu> cpu_;
};

} // namespace chip8

#endif // CHIP8_VIRTUALMACHINE_HPP
