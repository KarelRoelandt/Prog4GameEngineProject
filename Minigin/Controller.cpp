#include "Controller.h"
#include <iostream>
#include <cstring>

namespace dae
{
    Controller::Controller(int controllerIndex)
        : m_ControllerIndex(controllerIndex)
    {
        std::memset(&m_CurrentState, 0, sizeof(XINPUT_STATE));
        std::memset(&m_PreviousState, 0, sizeof(XINPUT_STATE));
    }

    void Controller::Update()
    {
        std::memcpy(&m_PreviousState, &m_CurrentState, sizeof(XINPUT_STATE));
        std::memset(&m_CurrentState, 0, sizeof(XINPUT_STATE));

        if (XInputGetState(m_ControllerIndex, &m_CurrentState) == ERROR_SUCCESS)
        {
            auto buttonChanges = m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons;
            m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
            m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);

            // Debug output
            //std::cout << "Controller " << m_ControllerIndex << " state updated.\n";
            //std::cout << "Buttons pressed this frame: " << m_ButtonsPressedThisFrame << "\n";
            //std::cout << "Buttons released this frame: " << m_ButtonsReleasedThisFrame << "\n";
        }
        else
        {
            //std::cout << "Controller " << m_ControllerIndex << " not connected.\n";
        }
    }

    bool Controller::IsButtonPressed(WORD button) const
    {
        return (m_ButtonsPressedThisFrame & button) != 0;
    }

    bool Controller::IsButtonReleased(WORD button) const
    {
        return (m_ButtonsReleasedThisFrame & button) != 0;
    }
}