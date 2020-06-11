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
#include <cstring>
#include <unordered_map>

#include "keyboard.hpp"

namespace chip8 {

namespace {

/// @brief Key mapping
const std::unordered_map<SDL_Keycode, size_t> KEYMAP = {
    { SDLK_0,  0 },
    { SDLK_1,  1 },
    { SDLK_2,  2 },
    { SDLK_3,  3 },
    { SDLK_4,  4 },
    { SDLK_5,  5 },
    { SDLK_6,  6 },
    { SDLK_7,  7 },
    { SDLK_8,  8 },
    { SDLK_9,  9 },
    { SDLK_a, 10 },
    { SDLK_b, 11 },
    { SDLK_c, 12 },
    { SDLK_d, 13 },
    { SDLK_e, 14 },
    { SDLK_f, 15 },
};

} // namespace

/// @brief Construct a keyboard implementation instance.
KeyboardImpl::KeyboardImpl()
    : quit_{ false }
{
    std::memset(&keys_, false, sizeof(bool) * KEY_COUNT);
}

/// @brief Destroy the keyboard instance.
KeyboardImpl::~KeyboardImpl()
{
}

/// @brief Is key currently pressed.
///
/// @param key  The key to check.
/// @return True when pressed, otherwise false.
bool KeyboardImpl::isKeyPressed(uint16_t key) const
{
    return keys_[key];
}

/// @brief Update keyboard events.
void KeyboardImpl::update()
{
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0)
    {
        if (event.type == SDL_QUIT)
        {
            quit_ = true;
        }

        if (event.type == SDL_KEYDOWN)
        {
            auto entry = KEYMAP.find(event.key.keysym.sym);

            if (entry != KEYMAP.end())
            {
                keys_[entry->second] = true;
            }
        }

        if (event.type == SDL_KEYUP)
        {
            auto entry = KEYMAP.find(event.key.keysym.sym);

            if (entry != KEYMAP.end())
            {
                keys_[entry->second] = false;
            }
        }
    }
}

} // namespace chip8
