#pragma once
#include <SDL.h>
#include <unordered_map>

namespace dae
{
    class Keyboard
    {
    public:
        Keyboard();
        void Update();
        bool IsKeyPressed(SDL_Keycode key) const;
        bool IsKeyReleased(SDL_Keycode key) const;

    private:
        std::unordered_map<SDL_Keycode, bool> m_CurrentState;
        std::unordered_map<SDL_Keycode, bool> m_PreviousState;
    };
}