#include "Keyboard.h"

namespace dae
{
    Keyboard::Keyboard()
    {
        // Initialize key states
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
            return prevIt == m_PreviousState.end() || !prevIt->second;
        }
        return false;
    }

    bool Keyboard::IsKeyReleased(SDL_Keycode key) const
    {
        auto it = m_CurrentState.find(key);
        if (it == m_CurrentState.end() || !it->second)
        {
            auto prevIt = m_PreviousState.find(key);
            return prevIt != m_PreviousState.end() && prevIt->second;
        }
        return false;
    }

    bool Keyboard::IsKeyDown(SDL_Keycode key) const
    {
        auto it = m_CurrentState.find(key);
        return it != m_CurrentState.end() && it->second;
    }
}