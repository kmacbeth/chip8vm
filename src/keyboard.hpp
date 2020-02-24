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
#ifndef CHIP8_KEYBOARD_HPP
#define CHIP8_KEYBOARD_HPP

#include <core.hpp>

namespace chip8 {

/// @brief Keyboard interface
class Keyboard
{
    public:
        static constexpr uint32_t KEY_COUNT = 16;

        virtual ~Keyboard() {}

        virtual void pressKey(uint16_t key) = 0;
        virtual void releaseKey(uint16_t key) = 0;
        virtual bool isKeyPressed(uint16_t key) const  = 0;
};

/// @brief Keyboard implementation
class KeyboardImpl : public Keyboard
{
    public:
        KeyboardImpl() { }
        ~KeyboardImpl() { }

        void pressKey(uint16_t key) override { }
        void releaseKey(uint16_t key) override { }
        bool isKeyPressed(uint16_t key) const override { return false; }
};

}  // chip8

#endif  // CHIP8_KEYBOARD_HPP
