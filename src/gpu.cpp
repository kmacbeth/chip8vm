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
#include <SDL2/SDL.h>

#include <memory.hpp>
#include "gpu.hpp"

namespace chip8 {

/// @brief Construct a framebuffer instance.
///
/// @param renderer  Reference to renderer to construct texture frame.
Framebuffer::Framebuffer(SDL_Renderer * renderer)
    : pixelbuffer_{ }
    , frame_{ nullptr }
{
    frame_ = SDL_CreateTexture(renderer,
                               PIXEL_FORMAT,
                               SDL_TEXTUREACCESS_STREAMING,
                               DISPLAY_WIDTH,
                               DISPLAY_HEIGHT);
}

/// @brief Destroy the framebuffer instance.
Framebuffer::~Framebuffer()
{
    SDL_DestroyTexture(frame_);
}

/// @brief Set pixel value.
///
/// @param x     X coordinate in display.
/// @param y     Y coordinate in display.
/// @param byte  Pixel value.
void Framebuffer::setPixel(uint8_t x, uint8_t y, uint8_t pixel)
{
    auto address = computeLinearAddress(x, y);
    pixelbuffer_[address] = (0xFFFFFF00 * pixel) | 0x000000FF;
}

/// @brief Get pixel value.
///
/// @param x     X coordinate in display.
/// @param y     Y coordinate in display.
/// @return Pixel value.
uint8_t Framebuffer::getPixel(uint8_t x, uint8_t y)
{
    auto address = computeLinearAddress(x, y);
    auto pixel = (pixelbuffer_[address] >> 8);
    return pixel & 0x1;
}

/// @brief Draw framebuffer to frame texture.
void Framebuffer::draw()
{
    SDL_UpdateTexture(frame_, nullptr, pixelbuffer_, DISPLAY_WIDTH * PIXEL_SIZE);
}

/// @brief Compute address from (x,y) coords.
///
/// @param x  X coordinate.
/// @param y  Y coordinate.
/// @return 16-bit address.
uint16_t Framebuffer::computeLinearAddress(uint8_t x, uint8_t y)
{
    // Wrap around screen
    x &= (DISPLAY_WIDTH - 1);
    y &= (DISPLAY_HEIGHT - 1);

    return DISPLAY_WIDTH * y + x;
}

/// @brief Construct a GPU instance with framebuffer.
///
/// @param renderer  Instance of gpu renderer.
GpuImpl::GpuImpl(SDL_Renderer * renderer)
    : renderer_{ renderer }
    , framebuffer_{ std::make_unique<Framebuffer>(renderer) }
{
}

/// @brief Destroy the GPU instance.
GpuImpl::~GpuImpl()
{
    SDL_DestroyRenderer(renderer_);
}

/// @brief Clear frame buffer.
void GpuImpl::clearFrame()
{
    for (size_t y = 0; y < Framebuffer::DISPLAY_HEIGHT; ++y)
    {
        for (size_t x = 0; x < Framebuffer::DISPLAY_WIDTH; ++x)
        {
            framebuffer_->setPixel(x, y, 0);
        }
    }

    draw();
}

/// @brief Draw a sprite.
///
/// @param x       X coordinate on display screen.
/// @param y       Y coordinate on display screen.
/// @param sprite  Spite 8xN
/// @return True when a pixel is erased, otherwise false.
bool GpuImpl::drawSprite(uint8_t x, uint8_t y, Sprite const& sprite)
{
    bool erased = false;

    for (size_t spriteY = 0; spriteY < sprite.size(); ++spriteY)
    {
        for (size_t spriteX = 0; spriteX < 8; ++spriteX)
        {
            uint8_t currentPixel = framebuffer_->getPixel(x + spriteX, y + spriteY);
            uint8_t spritePixel = (sprite[spriteY] >> (7 - spriteX)) & 0x1;

            if (currentPixel == 1 && spritePixel == 1)
            {
                erased = true;
            }

            currentPixel ^= spritePixel;

            framebuffer_->setPixel(x + spriteX, y + spriteY, currentPixel);
        }
    }

    return erased;
}

/// @brief Draw framebuffer to window.
void GpuImpl::draw()
{
    framebuffer_->draw();

    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, framebuffer_->frame(), nullptr, nullptr);
    SDL_RenderPresent(renderer_);
}

} // namespace chip8
