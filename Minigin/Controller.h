#pragma once
#include <windows.h>
#include <Xinput.h>

namespace dae
{
    class Controller
    {
    public:
        Controller(int controllerIndex);
        void Update();
        bool IsButtonPressed(WORD button) const;
        bool IsButtonReleased(WORD button) const;
        bool IsButtonDown(WORD button) const;
        int GetIndex() const { return m_ControllerIndex; }
        BYTE GetRightTriggerValue() const;
        BYTE GetLeftTriggerValue() const;

    private:
        int m_ControllerIndex;
        XINPUT_STATE m_CurrentState{};
        XINPUT_STATE m_PreviousState{};
        DWORD m_ButtonsPressedThisFrame{};
        DWORD m_ButtonsReleasedThisFrame{};
    };
}