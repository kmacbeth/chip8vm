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
#include <SDL2/SDL.h>

#include <memory.hpp>
#include <gpu.hpp>
#include <keyboard.hpp>
#include <cpu.hpp>

namespace chip8 {

namespace {

const uint32_t FPS = 60;
const uint32_t FRAME_DURATION = 1000 / 60;

}


class VirtualMachine
{
    public:
        VirtualMachine()
            : window_{ nullptr }
        {
        }

        ~VirtualMachine()
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
        bool initialize()
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
        void start()
        {
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

    private:
        SDL_Window * window_;

        std::shared_ptr<chip8::Gpu> gpu_;
        std::shared_ptr<chip8::Keyboard> keyboard_;
        std::shared_ptr<chip8::Memory> memory_;
        std::shared_ptr<chip8::Cpu> cpu_;
};

} // namespace chip8

int main(int argc, char * argv[])
{
    chip8::VirtualMachine chip8vm;

    if (!chip8vm.initialize())
    {
        return 1;
    }

    chip8vm.start();

    return 0;
}
