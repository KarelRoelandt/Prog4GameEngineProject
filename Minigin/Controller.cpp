// Controller.cpp
#include "Controller.h"
#include <Xinput.h>
#include <iostream>
#include <cstring>

namespace dae
{
    // Helper function to convert GamepadButton to XInput button code
    WORD GamepadButtonToXInput(GamepadButton button)
    {
        switch (button)
        {
        case GamepadButton::DPadUp:        return XINPUT_GAMEPAD_DPAD_UP;
        case GamepadButton::DPadDown:      return XINPUT_GAMEPAD_DPAD_DOWN;
        case GamepadButton::DPadLeft:      return XINPUT_GAMEPAD_DPAD_LEFT;
        case GamepadButton::DPadRight:     return XINPUT_GAMEPAD_DPAD_RIGHT;
        case GamepadButton::ButtonA:       return XINPUT_GAMEPAD_A;
        case GamepadButton::ButtonB:       return XINPUT_GAMEPAD_B;
        case GamepadButton::ButtonX:       return XINPUT_GAMEPAD_X;
        case GamepadButton::ButtonY:       return XINPUT_GAMEPAD_Y;
        case GamepadButton::LeftShoulder:  return XINPUT_GAMEPAD_LEFT_SHOULDER;
        case GamepadButton::RightShoulder: return XINPUT_GAMEPAD_RIGHT_SHOULDER;
        case GamepadButton::LeftThumb:     return XINPUT_GAMEPAD_LEFT_THUMB;
        case GamepadButton::RightThumb:    return XINPUT_GAMEPAD_RIGHT_THUMB;
        case GamepadButton::Start:         return XINPUT_GAMEPAD_START;
        case GamepadButton::Back:          return XINPUT_GAMEPAD_BACK;
        default:                          return 0;
        }
    }

    class Controller::ControllerImpl
    {
    public:
        ControllerImpl(int controllerIndex)
            : m_ControllerIndex(controllerIndex)
        {
            std::memset(&m_CurrentState, 0, sizeof(XINPUT_STATE));
            std::memset(&m_PreviousState, 0, sizeof(XINPUT_STATE));
        }

        void Update()
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

        bool IsButtonPressed(GamepadButton button) const
        {
            WORD xinputButton = GamepadButtonToXInput(button);
            return (m_ButtonsPressedThisFrame & xinputButton) != 0;
        }

        bool IsButtonReleased(GamepadButton button) const
        {
            WORD xinputButton = GamepadButtonToXInput(button);
            return (m_ButtonsReleasedThisFrame & xinputButton) != 0;
        }

        bool IsButtonDown(GamepadButton button) const
        {
            WORD xinputButton = GamepadButtonToXInput(button);
            return (m_CurrentState.Gamepad.wButtons & xinputButton) != 0;
        }

        BYTE GetRightTriggerValue() const
        {
            return m_CurrentState.Gamepad.bRightTrigger;
        }

        BYTE GetLeftTriggerValue() const
        {
            return m_CurrentState.Gamepad.bLeftTrigger;
        }

        // New analog stick methods in the implementation
        float GetLeftStickX() const
        {
            return m_CurrentState.Gamepad.sThumbLX / 32768.0f;
        }

        float GetLeftStickY() const
        {
            return m_CurrentState.Gamepad.sThumbLY / 32768.0f;
        }

        float GetRightStickX() const
        {
            return m_CurrentState.Gamepad.sThumbRX / 32768.0f;
        }

        float GetRightStickY() const
        {
            return m_CurrentState.Gamepad.sThumbRY / 32768.0f;
        }

        int m_ControllerIndex;
        XINPUT_STATE m_CurrentState{};
        XINPUT_STATE m_PreviousState{};
        DWORD m_ButtonsPressedThisFrame{};
        DWORD m_ButtonsReleasedThisFrame{};
    };

    Controller::Controller(int controllerIndex)
        : m_ControllerIndex(controllerIndex)
        , m_pImpl(new ControllerImpl(controllerIndex))
    {
    }

    Controller::~Controller()
    {
        delete m_pImpl;
    }

    Controller::Controller(Controller&& other) noexcept
        : m_ControllerIndex(other.m_ControllerIndex)
        , m_pImpl(other.m_pImpl)
    {
        other.m_pImpl = nullptr;
    }

    Controller& Controller::operator=(Controller&& other) noexcept
    {
        if (this != &other)
        {
            delete m_pImpl;
            m_ControllerIndex = other.m_ControllerIndex;
            m_pImpl = other.m_pImpl;
            other.m_pImpl = nullptr;
        }
        return *this;
    }

    void Controller::Update()
    {
        m_pImpl->Update();
    }

    bool Controller::IsButtonPressed(GamepadButton button) const
    {
        return m_pImpl->IsButtonPressed(button);
    }

    bool Controller::IsButtonReleased(GamepadButton button) const
    {
        return m_pImpl->IsButtonReleased(button);
    }

    bool Controller::IsButtonDown(GamepadButton button) const
    {
        return m_pImpl->IsButtonDown(button);
    }

    BYTE Controller::GetRightTriggerValue() const
    {
        return m_pImpl->GetRightTriggerValue();
    }

    BYTE Controller::GetLeftTriggerValue() const
    {
        return m_pImpl->GetLeftTriggerValue();
    }

    // New wrapper methods that delegate to the implementation
    float Controller::GetLeftStickX() const
    {
        return m_pImpl->GetLeftStickX();
    }

    float Controller::GetLeftStickY() const
    {
        return m_pImpl->GetLeftStickY();
    }

    float Controller::GetRightStickX() const
    {
        return m_pImpl->GetRightStickX();
    }

    float Controller::GetRightStickY() const
    {
        return m_pImpl->GetRightStickY();
    }
}