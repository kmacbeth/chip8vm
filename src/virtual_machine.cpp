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
#include <cstdio>
#include <iostream>
#include <unistd.h>

#include "virtual_machine.hpp"


namespace chip8 {

namespace {

/// @brief Fontset sprites.
uint8_t FONT_SET[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // Character 0
    0x20, 0x60, 0x20, 0x20, 0x70, // Character 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // Character 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // Character 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // Character 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // Character 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // Character 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // Character 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // Character 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // Character 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // Character A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // Character B
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // Character C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // Character D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // Character E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // Character F
};

const std::size_t FONT_SET_SIZE = sizeof(FONT_SET) / sizeof(FONT_SET[0]);

/// @brief Check file.
///
/// @param filename Filename to check.
bool checkFile(std::string const& filename)
{
    int fileStatus = ::access(filename.c_str(), F_OK);

    if (fileStatus == -1)
    {
        std::printf("No such file `%s'\n", filename.c_str());
        return false;
    }

    return true;
}

} // namespace

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
/// @param argc  Argument count.
/// @param argv  Argument list.
/// @return True when initialized, otherwise false.
bool VirtualMachine::initialize(int argc, char * argv[])
{
    // check for a file
    if (argc < 2)
    {
        std::puts("No file.");
        return false;
    }

    std::string filename{ argv[1] };

    if (!checkFile(filename))
    {
        return false;
    }

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

    loadFontset();
    loadFile(filename);

    return true;
}

/// @brief Start the virtual machine.
void VirtualMachine::start()
{
    const uint32_t FRAME_RATE = 30;
    const uint32_t FRAME_DURATION = 1000 / FRAME_RATE;
    const uint32_t CPU_CYCLE = 2;

    gpu_->clearFrame();
    cpu_->reset();

    uint32_t gpuTick = SDL_GetTicks();

    while (!keyboard_->isQuitRequested())
    {
        cpu_->tick(SDL_GetTicks());
        cpu_->update();

        keyboard_->update();

        uint32_t gpuTimeElapsed = SDL_GetTicks() - gpuTick;

        if (gpuTimeElapsed >= FRAME_DURATION)
        {
            gpu_->draw();
            gpuTick = SDL_GetTicks();
        }

        SDL_Delay(CPU_CYCLE);
    }
}

/// @brief Load fontset.
void VirtualMachine::loadFontset()
{
    // Add font table to memory (80 bytes)
    for (std::size_t fontAddress = 0; fontAddress < FONT_SET_SIZE; ++fontAddress)
    {
        memory_->store(fontAddress, FONT_SET[fontAddress]);
    }
}

/// @brief Load file to memory at start address.
void VirtualMachine::loadFile(std::string const& filename)
{
    std::FILE * programFile = std::fopen(filename.c_str(), "rb");
    std::size_t byteCount = 0;
    uint16_t address = Cpu::PROGRAM_START;

    do
    {
        uint8_t byte = 0;
        byteCount = std::fread(&byte, 1, 1, programFile);
        memory_->store(address++, byte);
    }
    while (byteCount != 0);

    std::fclose(programFile);
}

} // namespace chip8
