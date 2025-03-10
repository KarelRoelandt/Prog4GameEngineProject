#include "InputManager.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include <iostream>
#include <cmath> // For stick deadzone calculations

namespace dae
{
    class InputManager::Impl
    {
    public:
        Impl() :
            m_StickDeadZone(0.25f), // 25% deadzone is a good default
            m_StickRadialDeadZone(0.25f),
            m_AnalogAsDigital(true) // Default to treating analog as digital
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
                    //std::cout << "Key " << SDL_GetKeyName(key) << " down.\n";
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
                        //std::cout << "Controller " << controller.GetIndex() << ": Button down.\n";
                        HandleGamepadButtonEvent(button, InputState::Down);
                    }
                }

                // Process left stick input
                ProcessStickInput(controller, StickType::LeftStick);

                // Process right stick input
                ProcessStickInput(controller, StickType::RightStick);

                // Process triggers (can be added later)
                float leftTrigger = controller.GetLeftTriggerValue();
                float rightTrigger = controller.GetRightTriggerValue();

                if (leftTrigger > 0.1f) {
                    // Handle left trigger (could create a TriggerEvent handler)
                }

                if (rightTrigger > 0.1f) {
                    // Handle right trigger (could create a TriggerEvent handler)
                }
            }

            return true;
        }

        void ProcessStickInput(const Controller& controller, StickType stickType)
        {
            float x = 0.0f;
            float y = 0.0f;

            // Get stick values based on stick type
            if (stickType == StickType::LeftStick)
            {
                x = controller.GetLeftStickX();
                y = controller.GetLeftStickY();
            }
            else // RightStick
            {
                x = controller.GetRightStickX();
                y = controller.GetRightStickY();
            }

            // Calculate magnitude for radial deadzone
            float magnitude = std::sqrt(x * x + y * y);

            // Store the current stick state
            StickState currentState;
            currentState.x = x;
            currentState.y = y;
            currentState.magnitude = magnitude;
            currentState.angle = std::atan2(y, x) * 180.0f / 3.14159f; // Convert to degrees

            // Apply radial deadzone - if magnitude is less than deadzone, set both axes to zero
            if (magnitude < m_StickRadialDeadZone)
            {
                x = 0.0f;
                y = 0.0f;
                magnitude = 0.0f;
                currentState.direction = StickDirection::None;
            }
            else
            {
                // Determine direction
                // Use 8-way digital conversion if AnalogAsDigital is true
                if (m_AnalogAsDigital) {
                    float angle = currentState.angle;

                    // Convert angle to 8-way direction
                    if (angle >= -22.5f && angle < 22.5f)
                        currentState.direction = StickDirection::Right;
                    else if (angle >= 22.5f && angle < 67.5f)
                        currentState.direction = StickDirection::UpRight;
                    else if (angle >= 67.5f && angle < 112.5f)
                        currentState.direction = StickDirection::Up;
                    else if (angle >= 112.5f && angle < 157.5f)
                        currentState.direction = StickDirection::UpLeft;
                    else if (angle >= 157.5f || angle < -157.5f)
                        currentState.direction = StickDirection::Left;
                    else if (angle >= -157.5f && angle < -112.5f)
                        currentState.direction = StickDirection::DownLeft;
                    else if (angle >= -112.5f && angle < -67.5f)
                        currentState.direction = StickDirection::Down;
                    else if (angle >= -67.5f && angle < -22.5f)
                        currentState.direction = StickDirection::DownRight;
                }
                else {
                    // For true analog mode, we'll still set a primary direction
                    // but we'll pass the raw x,y values to the command
                    if (std::abs(x) > std::abs(y)) {
                        currentState.direction = (x > 0) ? StickDirection::Right : StickDirection::Left;
                    }
                    else {
                        currentState.direction = (y > 0) ? StickDirection::Up : StickDirection::Down;
                    }
                }

                // Normalize the stick input (optional)
                if (magnitude > 1.0f) {
                    x /= magnitude;
                    y /= magnitude;
                }
            }

            // Update stick state cache
            if (stickType == StickType::LeftStick) {
                m_LeftStickState = currentState;
            }
            else {
                m_RightStickState = currentState;
            }

            // Fire stick events if state has changed
            if (currentState.direction != StickDirection::None) {
                HandleStickEvent(stickType, currentState.direction, x, y, magnitude);
            }

            // Update previous stick states for detecting changes
            if (stickType == StickType::LeftStick)
            {
                m_PrevLeftStickX = x;
                m_PrevLeftStickY = y;
            }
            else // RightStick
            {
                m_PrevRightStickX = x;
                m_PrevRightStickY = y;
            }
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

        void BindStickCommand(StickType stickType, StickDirection direction, std::shared_ptr<Command> command)
        {
            // Create a unique key from stick type and direction
            int key = static_cast<int>(stickType) * 100 + static_cast<int>(direction);
            stickCommands[key] = std::move(command);
        }

        void SetStickDeadZone(float deadZone)
        {
            m_StickDeadZone = deadZone;
            m_StickRadialDeadZone = deadZone;
        }

        void SetStickRadialDeadZone(float deadZone)
        {
            m_StickRadialDeadZone = deadZone;
        }

        void SetAnalogAsDigital(bool useAsDigital)
        {
            m_AnalogAsDigital = useAsDigital;
        }

        const StickState& GetStickState(StickType stickType) const
        {
            return (stickType == StickType::LeftStick) ? m_LeftStickState : m_RightStickState;
        }

    private:
        static const int MAX_CONTROLLERS = 4;
        std::vector<Controller> m_Controllers;
        Keyboard m_Keyboard;
        float m_StickDeadZone;
        float m_StickRadialDeadZone;
        bool m_AnalogAsDigital;

        // Current stick states
        StickState m_LeftStickState;
        StickState m_RightStickState;

        // Previous stick values for detecting changes
        float m_PrevLeftStickX = 0.0f;
        float m_PrevLeftStickY = 0.0f;
        float m_PrevRightStickX = 0.0f;
        float m_PrevRightStickY = 0.0f;

        std::unordered_map<int, std::unordered_map<InputState, std::shared_ptr<Command>>> keyCommands;
        std::unordered_map<int, std::shared_ptr<Command>> gamepadCommands;
        std::unordered_map<int, std::shared_ptr<Command>> stickCommands;

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

        void HandleStickEvent(StickType stickType, StickDirection direction, float x, float y, float magnitude)
        {
            int key = static_cast<int>(stickType) * 100 + static_cast<int>(direction);
            auto commandIt = stickCommands.find(key);
            if (commandIt != stickCommands.end())
            {
                // Create a struct to pass all stick data to the command
                StickState state;
                state.x = x;
                state.y = y;
                state.magnitude = magnitude;
                state.direction = direction;

                // Pass stick value to command through Execute with extended parameter support
                if (m_AnalogAsDigital) {
                    // Just pass a normalized value if using digital mode
                    commandIt->second->Execute(magnitude);
                }
                else {
                    // Pass both x and y values if using true analog mode
                    commandIt->second->Execute(x, y);
                }
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

    void InputManager::BindStickCommand(StickType stickType, StickDirection direction, std::shared_ptr<Command> command)
    {
        pImpl->BindStickCommand(stickType, direction, std::move(command));
    }

    void InputManager::SetStickDeadZone(float deadZone)
    {
        pImpl->SetStickDeadZone(deadZone);
    }

    // New public methods to support enhanced stick functionality
    void InputManager::SetStickRadialDeadZone(float deadZone)
    {
        pImpl->SetStickRadialDeadZone(deadZone);
    }

    void InputManager::SetAnalogAsDigital(bool useAsDigital)
    {
        pImpl->SetAnalogAsDigital(useAsDigital);
    }

    // Method to get the current state of a joystick
    StickState InputManager::GetStickState(StickType stickType) const
    {
        return pImpl->GetStickState(stickType);
    }
}