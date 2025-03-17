#pragma once
#include "Singleton.h"
#include "Command.h"
#include "Controller.h"
#include "Keyboard.h"
#include <SDL.h>
#include <vector>
#include <unordered_map>
#include <memory>

namespace dae
{
    enum class InputState
    {
        Pressed,
        Released,
        Down
    };

    class InputManager final : public Singleton<InputManager>
    {
    public:
        InputManager();
        ~InputManager();
        bool ProcessInput();
        void BindCommand(int key, InputState state, std::shared_ptr<Command> command);
        void BindControllerCommand(GamepadButton button, InputState state, std::shared_ptr<Command> command);
        // Methods for stick command binding
        void BindStickCommand(StickType stickType, StickDirection direction, std::shared_ptr<Command> command);
        // Configure stick deadzone
        void SetStickDeadZone(float deadZone);
        // New methods for enhanced stick functionality
        void SetStickRadialDeadZone(float deadZone);
        void SetAnalogAsDigital(bool useAsDigital);
        StickState GetStickState(StickType stickType) const;
    private:
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };
}