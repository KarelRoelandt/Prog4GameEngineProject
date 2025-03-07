#pragma once
#include "Singleton.h"
#include "Command.h"
#include "Controller.h"
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
        InputManager(); // Constructor declaration
        bool ProcessInput();
        void BindCommand(int key, InputState state, std::shared_ptr<Command> command);
        void BindControllerCommand(int button, InputState state, std::shared_ptr<Command> command);

    private:
        static const int MAX_CONTROLLERS = 4;
        std::vector<Controller> m_Controllers;

        std::unordered_map<int, std::unordered_map<InputState, std::shared_ptr<Command>>> keyCommands;
        std::unordered_map<int, std::unordered_map<InputState, std::shared_ptr<Command>>> controllerCommands;

        void HandleKeyEvent(int key, InputState state);
        void HandleControllerEvent(int button, InputState state);
    };
}