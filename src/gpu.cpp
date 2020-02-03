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
#include <memory.hpp>
#include "gpu.hpp"

namespace chip8 {


/// @brief Construct a GPU instance with framebuffer.
///
/// @param frameBuffer Framebuffer memory.
GpuImpl::GpuImpl(Memory & framebuffer)
    : framebuffer_(framebuffer)
{
}

/// @brief Destroy the GPU instance.
GpuImpl::~GpuImpl()
{
}

/// @brief Clear framebuffer.
void GpuImpl::clearFramebuffer()
{
    for (size_t address = 0; address < framebuffer_.getSize(); ++address)
    {
        framebuffer_.store(address, 0x00);
    }
}

/// @brief Draw a sprite.
///
/// @param x  X coordinate on display screen.
/// @param y  Y coordinate on display screen.
/// @return Pixel was erased.
bool GpuImpl::drawSprite(uint8_t x, uint8_t y, Sprite const& sprite)
{
    bool erased = false;

    for (size_t spriteIndex = 0; spriteIndex < sprite.size(); ++spriteIndex)
    {
        uint8_t yPixel = y + spriteIndex;

        for (uint8_t bitIndex = 0x80, xPixel = x; bitIndex != 0; bitIndex >>= 1, ++xPixel)
        {
            uint8_t spriteValue = (sprite[spriteIndex] & bitIndex) ? 0xFF : 0x00;

            uint16_t address = computeLinearAddress(xPixel, yPixel);
            uint8_t pixel = framebuffer_.load<uint8_t>(address) ^ spriteValue;

#if 0
            std::printf("Address 0x%04X    Sprite Value %02X    Pixel %02X    (x,y) = (%u,%u)\n", address, spriteValue, pixel, xPixel, yPixel);
#endif

            if (pixel == 0)
            {
                erased = true;
            }

            framebuffer_.store(address, pixel);
        }
    }

    return erased;
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
