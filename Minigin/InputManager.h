#pragma once
#include "Singleton.h"
#include <windows.h>
#include <Xinput.h>
#include <SDL.h>
#include <array>

namespace dae {
    class InputManager final : public Singleton<InputManager> {
    public:
        bool ProcessInput();
    private:
        static const int MAX_CONTROLLERS = 4;
        std::array<XINPUT_STATE, MAX_CONTROLLERS> currentStates{};
        std::array<XINPUT_STATE, MAX_CONTROLLERS> previousStates{};
        std::array<DWORD, MAX_CONTROLLERS> buttonsPressedThisFrame{};
        std::array<DWORD, MAX_CONTROLLERS> buttonsReleasedThisFrame{};

        bool IsButtonAPressed(const XINPUT_GAMEPAD& gamepad);
        bool IsButtonBPressed(const XINPUT_GAMEPAD& gamepad);
    };
}