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
#ifndef CHIP8_FAKEKEYBOARD_HPP
#define CHIP8_FAKEKEYBOARD_HPP

#include <keyboard.hpp>

namespace chip8 {

/// @brief Fake GPU.
class FakeKeyboard : public Keyboard
{
    public:
        FakeKeyboard()
            : keys()
        {
            std::fill(keys, keys + KEY_COUNT, false);
        }

        ~FakeKeyboard()
        {
        }

        void pressKey(uint16_t key)
        {
            keys[key] = true;
        }

        void releaseKey(uint16_t key)
        {
            keys[key] = false;
        }


        bool isQuitRequested() const override
        {
            return false;
        }

        bool isKeyPressed(uint16_t key) const override
        {
            return keys[key];
        }

        void update() override { }

        bool keys[KEY_COUNT];
};

}  // chip8

#endif  // CHIP8_FAKEKEYBOARD_HPP
