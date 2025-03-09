#include "InputManager.h"
#include <XInput.h> // Include XInput here
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include <iostream>

namespace dae
{
    class InputManager::Impl
    {
    public:
        Impl()
        {
            for (int i = 0; i < MAX_CONTROLLERS; ++i)
            {
                m_Controllers.emplace_back(i);
            }
        }

        bool ProcessInput()
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
                if (m_Keyboard.IsKeyDown(key))
                {
                    std::cout << "Key " << SDL_GetKeyName(key) << " down.\n";
                    HandleKeyEvent(key, InputState::Down);
                }
            }

            // Update controller states
            for (auto& controller : m_Controllers)
            {
                controller.Update();

                // Define an array of all GamepadButton values
                const GamepadButton buttonsToCheck[] = {
                    GamepadButton::DPadUp,
                    GamepadButton::DPadDown,
                    GamepadButton::DPadLeft,
                    GamepadButton::DPadRight,
                    GamepadButton::ButtonA,
                    GamepadButton::ButtonB,
                    GamepadButton::ButtonX,
                    GamepadButton::ButtonY,
                    GamepadButton::LeftShoulder,
                    GamepadButton::RightShoulder,
                    GamepadButton::LeftThumb,
                    GamepadButton::RightThumb,
                    GamepadButton::Start,
                    GamepadButton::Back
                };

                // Check each button state
                for (const auto& button : buttonsToCheck)
                {
                    if (controller.IsButtonPressed(button))
                    {
                        std::cout << "Controller " << controller.GetIndex() << ": Button pressed.\n";
                        HandleGamepadButtonEvent(button, InputState::Pressed);
                    }
                    if (controller.IsButtonReleased(button))
                    {
                        std::cout << "Controller " << controller.GetIndex() << ": Button released.\n";
                        HandleGamepadButtonEvent(button, InputState::Released);
                    }
                    if (controller.IsButtonDown(button))
                    {
                        std::cout << "Controller " << controller.GetIndex() << ": Button down.\n";
                        HandleGamepadButtonEvent(button, InputState::Down);
                    }
                }

                // Debug output for triggers can remain as is
                //std::cout << "Controller " << controller.GetIndex() << ": RT value = " << static_cast<int>(controller.GetRightTriggerValue()) << "\n";
                //std::cout << "Controller " << controller.GetIndex() << ": LT value = " << static_cast<int>(controller.GetLeftTriggerValue()) << "\n";
            }

            return true;
        }

        void BindCommand(int key, InputState state, std::shared_ptr<Command> command)
        {
            keyCommands[key][state] = std::move(command);
        }

        void BindGamepadButtonCommand(GamepadButton button, InputState state, std::shared_ptr<Command> command)
        {
            // Use a unique key combining button and state
            int key = static_cast<int>(button) * 10 + static_cast<int>(state);
            gamepadCommands[key] = std::move(command);
        }

    private:
        static const int MAX_CONTROLLERS = 4;
        std::vector<Controller> m_Controllers;
        Keyboard m_Keyboard;

        std::unordered_map<int, std::unordered_map<InputState, std::shared_ptr<Command>>> keyCommands;
        std::unordered_map<int, std::shared_ptr<Command>> gamepadCommands;

        void HandleKeyEvent(int key, InputState state)
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

        void HandleGamepadButtonEvent(GamepadButton button, InputState state)
        {
            // Create the key using the same formula as in BindGamepadButtonCommand
            int key = static_cast<int>(button) * 10 + static_cast<int>(state);
            auto commandIt = gamepadCommands.find(key);
            if (commandIt != gamepadCommands.end())
            {
                commandIt->second->Execute();
            }
        }
    };

    InputManager::InputManager()
        : pImpl(std::make_unique<Impl>())
    {
    }

    InputManager::~InputManager() = default;

    bool InputManager::ProcessInput()
    {
        return pImpl->ProcessInput();
    }

    void InputManager::BindCommand(int key, InputState state, std::shared_ptr<Command> command)
    {
        pImpl->BindCommand(key, state, std::move(command));
    }

    void InputManager::BindControllerCommand(GamepadButton button, InputState state, std::shared_ptr<Command> command)
    {
        // Using the PIMPL pattern to delegate to the implementation
        pImpl->BindGamepadButtonCommand(button, state, std::move(command));
    }
}