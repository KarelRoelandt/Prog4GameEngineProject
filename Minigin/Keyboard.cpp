#include "Keyboard.h"

#include <iostream>

namespace dae
{
    Keyboard::Keyboard()
    {
        // Initialize key states
            
        const Uint8* state = SDL_GetKeyboardState(nullptr);
        for (int i = 0; i < SDL_NUM_SCANCODES; ++i)
        {
            SDL_Keycode key = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(i));
            m_CurrentState[key] = state[i];
            m_PreviousState[key] = 0; // Initialize previous state to 0 (not pressed)
        }
    }

    void Keyboard::Update()
    {
        m_PreviousState = m_CurrentState;
        m_CurrentState.clear();

        const Uint8* state = SDL_GetKeyboardState(nullptr);
        for (int i = 0; i < SDL_NUM_SCANCODES; ++i)
        {
            SDL_Keycode key = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(i));
            m_CurrentState[key] = state[i];
        }
    }

    bool Keyboard::IsKeyPressed(SDL_Keycode key) const
    {
        auto it = m_CurrentState.find(key);
        if (it != m_CurrentState.end() && it->second)
        {
            auto prevIt = m_PreviousState.find(key);
            bool isPressed = prevIt == m_PreviousState.end() || !prevIt->second;
            if (isPressed)
            {
                std::cout << "Key " << SDL_GetKeyName(key) << " pressed.\n";
            }
            return isPressed;
        }
        return false;
    }

    bool Keyboard::IsKeyReleased(SDL_Keycode key) const
    {
        auto it = m_CurrentState.find(key);
        if (it == m_CurrentState.end() || !it->second)
        {
            auto prevIt = m_PreviousState.find(key);
            bool isReleased = prevIt != m_PreviousState.end() && prevIt->second;
            if (isReleased)
            {
                std::cout << "Key " << SDL_GetKeyName(key) << " released.\n";
            }
            return isReleased;
        }
        return false;
    }

    bool Keyboard::IsKeyDown(SDL_Keycode key) const
    {
        auto it = m_CurrentState.find(key);
        bool isDown = it != m_CurrentState.end() && it->second;
        if (isDown)
        {
            std::cout << "Key " << SDL_GetKeyName(key) << " down.\n";
        }
        return isDown;
    }
}
