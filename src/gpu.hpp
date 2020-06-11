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

#include <array>
#include <SDL2/SDL.h>

#include <core.hpp>
#include <memory.hpp>

namespace chip8 {

using Sprite = Memory::Bytes;

class GpuImpl;

class Framebuffer
{
    public:
        Framebuffer(SDL_Renderer * renderer);
        ~Framebuffer();

        SDL_Texture * frame() { return frame_; }

        uint8_t getPixel(uint8_t x, uint8_t y);
        void    setPixel(uint8_t x, uint8_t y, uint8_t byte);

        void draw();

    private:
        friend GpuImpl;
        using Pixel = uint32_t;

        static const uint32_t PIXEL_FORMAT = SDL_PIXELFORMAT_RGBA8888;
        static const size_t PIXEL_SIZE = sizeof(Pixel);

        static const uint32_t DISPLAY_WIDTH = 64;
        static const uint32_t DISPLAY_HEIGHT = 32;

        /// @brief GPU framebuffer size.
        static constexpr uint16_t FRAMEBUFFER_SIZE = DISPLAY_WIDTH * DISPLAY_HEIGHT;

        static uint16_t computeLinearAddress(uint8_t x, uint8_t y);

        SDL_Rect       frameRect_;
        SDL_Texture *  frame_;

        Pixel pixelbuffer_[FRAMEBUFFER_SIZE];
};

/// @brief Represent the CHIP-8 GPU.
class Gpu
{
    public:
        virtual ~Gpu() {}

        virtual void clearFrame() = 0;
        virtual bool drawSprite(uint8_t x, uint8_t y, Sprite const& sprite) = 0;
        virtual void draw() = 0;
};

/// @brief Represent the CHIP-8 GPU implementation.
class GpuImpl : public Gpu
{
    public:
        GpuImpl(SDL_Renderer * renderer);
        ~GpuImpl();

        void clearFrame() override;
        bool drawSprite(uint8_t x, uint8_t y, Sprite const& sprite) override;
        void draw() override;

    private:
        SDL_Renderer * renderer_;
        std::unique_ptr<Framebuffer> framebuffer_;
};

}  // chip8

#endif  // CHIP8_GPU_HPP

