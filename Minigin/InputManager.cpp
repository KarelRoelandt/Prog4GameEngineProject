#include "InputManager.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include <iostream>

namespace dae
{
    InputManager::InputManager()
    {
        for (int i = 0; i < MAX_CONTROLLERS; ++i)
        {
            m_Controllers.emplace_back(i);
        }
    }

    void InputManager::BindCommand(int key, InputState state, std::shared_ptr<Command> command)
    {
        keyCommands[key][state] = command;
    }

    void InputManager::BindControllerCommand(int button, InputState state, std::shared_ptr<Command> command)
    {
        controllerCommands[button][state] = command;
    }

    void InputManager::HandleKeyEvent(int key, InputState state)
    {
        auto keyIt = keyCommands.find(key);
        if (keyIt != keyCommands.end())
        {
            auto stateIt = keyIt->second.find(state);
            if (stateIt != keyIt->second.end())
            {
                stateIt->second->Execute();
            }
        }
    }

    void InputManager::HandleControllerEvent(int button, InputState state)
    {
        auto buttonIt = controllerCommands.find(button);
        if (buttonIt != controllerCommands.end())
        {
            auto stateIt = buttonIt->second.find(state);
            if (stateIt != buttonIt->second.end())
            {
                stateIt->second->Execute();
            }
        }
    }

    bool InputManager::ProcessInput()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                return false;
            }
            if (e.type == SDL_KEYDOWN)
            {
                HandleKeyEvent(e.key.keysym.sym, InputState::Pressed);
            }
            if (e.type == SDL_KEYUP)
            {
                HandleKeyEvent(e.key.keysym.sym, InputState::Released);
            }
            // etc...

            // Event for ImGui
            ImGui_ImplSDL2_ProcessEvent(&e);
        }

        // Update keyboard state
        m_Keyboard.Update();

        // Check key states and execute commands
        const std::vector<SDL_Keycode> keys = { SDLK_w, SDLK_a, SDLK_s, SDLK_d, SDLK_z, SDLK_q };
        for (const auto& key : keys)
        {
            if (m_Keyboard.IsKeyPressed(key))
            {
                std::cout << "Key " << SDL_GetKeyName(key) << " pressed.\n";
                HandleKeyEvent(key, InputState::Pressed);
            }
            if (m_Keyboard.IsKeyReleased(key))
            {
                std::cout << "Key " << SDL_GetKeyName(key) << " released.\n";
                HandleKeyEvent(key, InputState::Released);
            }
        }

        // Update controller states
        for (auto& controller : m_Controllers)
        {
            controller.Update();

            // Check button states and execute commands
            const std::vector<std::pair<WORD, const char*>> buttons = {
                {static_cast<WORD>(XINPUT_GAMEPAD_A), "A"},
                {static_cast<WORD>(XINPUT_GAMEPAD_B), "B"},
                {static_cast<WORD>(XINPUT_GAMEPAD_X), "X"},
                {static_cast<WORD>(XINPUT_GAMEPAD_Y), "Y"},
                {static_cast<WORD>(XINPUT_GAMEPAD_DPAD_UP), "D-Pad Up"},
                {static_cast<WORD>(XINPUT_GAMEPAD_DPAD_DOWN), "D-Pad Down"},
                {static_cast<WORD>(XINPUT_GAMEPAD_DPAD_LEFT), "D-Pad Left"},
                {static_cast<WORD>(XINPUT_GAMEPAD_DPAD_RIGHT), "D-Pad Right"},
                {static_cast<WORD>(XINPUT_GAMEPAD_LEFT_SHOULDER), "Left Shoulder"},
                {static_cast<WORD>(XINPUT_GAMEPAD_RIGHT_SHOULDER), "Right Shoulder"},
                {static_cast<WORD>(XINPUT_GAMEPAD_LEFT_THUMB), "Left Thumb"},
                {static_cast<WORD>(XINPUT_GAMEPAD_RIGHT_THUMB), "Right Thumb"},
                {static_cast<WORD>(XINPUT_GAMEPAD_START), "Start"},
                {static_cast<WORD>(XINPUT_GAMEPAD_BACK), "Back"}
            };

            for (const auto& [button, name] : buttons)
            {
                if (controller.IsButtonPressed(button))
                {
                    std::cout << "Controller " << controller.GetIndex() << ": " << name << " button pressed.\n";
                    HandleControllerEvent(button, InputState::Pressed);
                }
                if (controller.IsButtonReleased(button))
                {
                    std::cout << "Controller " << controller.GetIndex() << ": " << name << " button released.\n";
                    HandleControllerEvent(button, InputState::Released);
                }
            }

            // Debug output for triggers
            //std::cout << "Controller " << controller.GetIndex() << ": RT value = " << static_cast<int>(controller.GetRightTriggerValue()) << "\n";
            //std::cout << "Controller " << controller.GetIndex() << ": LT value = " << static_cast<int>(controller.GetLeftTriggerValue()) << "\n";
        }

        return true;
    }
}