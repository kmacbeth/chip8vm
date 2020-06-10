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
#ifndef CHIP8_GPU_HPP
#define CHIP8_GPU_HPP

#include <SDL2/SDL.h>

#include <core.hpp>
#include <memory.hpp>

namespace chip8 {

using Sprite = Memory::Bytes;

/// @brief Represent the CHIP-8 GPU.
class Gpu
{
    public:
        static const uint16_t DISPLAY_WIDTH = 64;
        static const uint16_t DISPLAY_HEIGHT = 32;

        virtual ~Gpu() {}

        virtual void clearFrame() = 0;
        virtual bool drawSprite(uint8_t x, uint8_t y, Sprite const& sprite) = 0;
        virtual void draw() = 0;
};

/// @brief Represent the CHIP-8 GPU implementation.
class GpuImpl : public Gpu
{
    public:
        GpuImpl(SDL_Window * window);
        ~GpuImpl();

        void clearFrame();
        bool drawSprite(uint8_t x, uint8_t y, Sprite const& sprite) override;
        void draw() override;

    private:
        static uint16_t computeLinearAddress(uint8_t x, uint8_t y);

        SDL_Window  *  window_;
        SDL_Renderer * renderer_;
        SDL_Rect       viewport_;
        SDL_Texture *  frame_;
        uint32_t *     pixels_;
};

}  // chip8

#endif  // CHIP8_GPU_HPP

