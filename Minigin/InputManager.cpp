#include "InputManager.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include <iostream>

bool dae::InputManager::IsButtonAPressed(const XINPUT_GAMEPAD& gamepad) {
    return ((gamepad.wButtons & XINPUT_GAMEPAD_A) != 0);
}

bool dae::InputManager::IsButtonBPressed(const XINPUT_GAMEPAD& gamepad) {
    return ((gamepad.wButtons & XINPUT_GAMEPAD_B) != 0);
}

bool dae::InputManager::ProcessInput() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return false;
        }
        if (e.type == SDL_KEYDOWN) {
            // Handle key down events
        }
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            // Handle mouse button down events
        }
        // etc...

        // Event for ImGui
        ImGui_ImplSDL2_ProcessEvent(&e);
    }

    // Update controller states
    for (int i = 0; i < MAX_CONTROLLERS; ++i) {
        CopyMemory(&previousStates[i], &currentStates[i], sizeof(XINPUT_STATE));
        ZeroMemory(&currentStates[i], sizeof(XINPUT_STATE));

        if (XInputGetState(i, &currentStates[i]) == ERROR_SUCCESS) {
            auto buttonChanges = currentStates[i].Gamepad.wButtons ^ previousStates[i].Gamepad.wButtons;
            buttonsPressedThisFrame[i] = buttonChanges & currentStates[i].Gamepad.wButtons;
            buttonsReleasedThisFrame[i] = buttonChanges & (~currentStates[i].Gamepad.wButtons);

            // Example: Check if the A button was pressed this frame
            if (buttonsPressedThisFrame[i] & XINPUT_GAMEPAD_A) {
                // Handle A button press
                std::cout << "AP" << i << "\n";
            }

            // Example: Check if the A button was released this frame
            if (buttonsReleasedThisFrame[i] & XINPUT_GAMEPAD_A) {
                // Handle A button release
                std::cout << "AL" << i << "\n";
            }

            // Example: Check if the B button was pressed this frame
            if (buttonsPressedThisFrame[i] & XINPUT_GAMEPAD_B) {
                // Handle B button press
                std::cout << "BP" << i << "\n";
            }

            // Example: Check if the B button was released this frame
            if (buttonsReleasedThisFrame[i] & XINPUT_GAMEPAD_B) {
                // Handle B button release
                std::cout << "BL" << i << "\n";
            }
        }
    }

    return true;
}