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
#ifndef CHIP8_FAKEGPU_HPP
#define CHIP8_FAKEGPU_HPP

#include <gpu.hpp>

namespace chip8 {

/// @brief Fake GPU.
class FakeGpu : public Gpu
{
    public:
        struct DrawContext
        {
            uint8_t x;
            uint8_t y;
            Sprite sprite;
        };

        FakeGpu() = default;
        ~FakeGpu() {}

        void clearFramebuffer() override
        {
            ++clearCount;
        }

        bool drawSprite(uint8_t x, uint8_t y, Sprite const& sprite) override
        {
            drawContext.x = x;
            drawContext.y = y;
            drawContext.sprite = sprite;
            return spriteErased;
        }

        uint16_t clearCount = 0;
        DrawContext drawContext;
        bool spriteErased = false;
};

}  // chip8

#endif  // CHIP8_FAKEGPU_HPP