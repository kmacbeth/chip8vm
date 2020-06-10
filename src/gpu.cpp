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
#include <cstdlib>
#include <cstring>

#include <SDL2/SDL.h>

#include <memory.hpp>
#include "gpu.hpp"

namespace chip8 {


/// @brief Construct a GPU instance with framebuffer.
///
/// @param window  SDL window.
/// @param frame   SDL surface for screen.
GpuImpl::GpuImpl(SDL_Window * window)
    : window_{ window }
    , renderer_{ nullptr }
    , viewport_{ }
    , frame_{ nullptr }
    , pixels_{ nullptr }
{
    renderer_ = SDL_CreateRenderer(window_, -1, 0);

    SDL_RenderGetViewport(renderer_, &viewport_);

    // Check https://github.com/JamesGriffin/CHIP-8-Emulator/blob/master/src/main.cpp
    frame_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, viewport_.w, viewport_.h);
    pixels_ = static_cast<uint32_t *>(std::malloc(sizeof(uint32_t) * viewport_.w * viewport_.h));

    std::memset(pixels_, 0x00, sizeof(uint32_t) * viewport_.w * viewport_.h);
}

/// @brief Destroy the GPU instance.
GpuImpl::~GpuImpl()
{
    SDL_DestroyWindow(window_);
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyTexture(frame_);
    std::free(pixels_);
}

void GpuImpl::clearFrame()
{
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
}

/// @brief Draw a sprite.
///
/// @param x  X coordinate on display screen.
/// @param y  Y coordinate on display screen.
/// @return Pixel was erased.
bool GpuImpl::drawSprite(uint8_t x, uint8_t y, Sprite const& sprite)
{
    bool erased = false;

#if 0
    for (size_t spriteIndex = 0; spriteIndex < sprite.size(); ++spriteIndex)
    {
        uint8_t yPixel = y + spriteIndex;

        for (uint8_t bitIndex = 0x80, xPixel = x; bitIndex != 0; bitIndex >>= 1, ++xPixel)
        {
            uint8_t spriteValue = (sprite[spriteIndex] & bitIndex) ? 0xFF : 0x00;

            uint16_t address = computeLinearAddress(xPixel, yPixel);
            uint8_t pixel = framebuffer_.load<uint8_t>(address) ^ spriteValue;

            std::printf("Address 0x%04X    Sprite Value %02X    Pixel %02X    (x,y) = (%u,%u)\n", address, spriteValue, pixel, xPixel, yPixel);

            if (pixel == 0)
            {
                erased = true;
            }

            framebuffer_.store(address, pixel);
        }
    }
#endif

    return erased;
}

/// @brief Draw framebuffer to window.
void GpuImpl::draw()
{

    pixels_[2 * viewport_.w * sizeof(uint32_t) + 0] = 0xFFFFFF00;
    pixels_[2 * viewport_.w * sizeof(uint32_t) + 1] = 0xFFFFFF00;
    pixels_[2 * viewport_.w * sizeof(uint32_t) + 2] = 0xFFFFFF00;
    pixels_[2 * viewport_.w * sizeof(uint32_t) + 3] = 0xFFFFFF00;
    pixels_[2 * viewport_.w * sizeof(uint32_t) + 4] = 0xFFFFFF00;
    pixels_[2 * viewport_.w * sizeof(uint32_t) + 5] = 0xFFFFFF00;

    SDL_UpdateTexture(frame_, NULL, pixels_, viewport_.w * sizeof(uint32_t));

    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, frame_, NULL, NULL);
    SDL_RenderPresent(renderer_);
}

/// @brief Compute address from (x,y) coords.
///
/// @param x X coordinate.
/// @param y Y coordinate.
/// @return 16-bit address.
uint16_t GpuImpl::computeLinearAddress(uint8_t x, uint8_t y)
{
    // Wrap around screen
    x &= (DISPLAY_WIDTH-1);
    y &= (DISPLAY_HEIGHT-1);

    return DISPLAY_WIDTH * y + x;
}

}  // chip8
