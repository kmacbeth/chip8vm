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
#include "virtual_machine.hpp"

namespace chip8 {

/// @brief Construct a CHIP-8 VM instance.
VirtualMachine::VirtualMachine()
    : window_{ nullptr }
{
}

/// @brief Destroy a CHIP-8
VirtualMachine::~VirtualMachine()
{
    if (window_ != nullptr)
    {
        SDL_DestroyWindow(window_);
    }

    SDL_Quit();
}

/// @brief Initialize the virtual machine.
///
/// @return True when initialized, otherwise false.
bool VirtualMachine::initialize()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        return false;
    }

    const int WIN_WIDTH = 1024;
    const int WIN_HEIGHT = 512;

    window_ = SDL_CreateWindow("CHIP-8",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            WIN_WIDTH,
            WIN_HEIGHT,
            SDL_WINDOW_SHOWN);

    if (window_ == nullptr)
    {
        return false;
    }

    SDL_Renderer * renderer = SDL_CreateRenderer(window_, -1, 0);

    gpu_ = std::make_shared<chip8::GpuImpl>(renderer);
    keyboard_ = std::make_shared<chip8::KeyboardImpl>();
    memory_ = std::make_shared<chip8::Memory>(chip8::SYSTEM_MEMORY_SIZE);
    cpu_ = std::make_shared<chip8::CpuImpl>(memory_, keyboard_, gpu_);

    return true;
}

/// @brief Start the virtual machine.
void VirtualMachine::start()
{
    const uint32_t FRAME_DURATION = 60;

    gpu_->clearFrame();

    while (!keyboard_->isQuitRequested())
    {
        uint32_t startTick = SDL_GetTicks();

        // TODO: add cpu

        keyboard_->update();
        gpu_->draw();

        uint32_t currentDuration = SDL_GetTicks() - startTick;

        if (currentDuration < FRAME_DURATION)
        {
            SDL_Delay(FRAME_DURATION - currentDuration);
        }
    }
}

} // namespace chip8
