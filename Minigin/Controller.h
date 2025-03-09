// Controller.h
#pragma once
#include <windows.h>
// Forward declarations for Windows types
typedef unsigned short WORD;
typedef unsigned char BYTE;

namespace dae
{
    // Game-specific button enum that doesn't depend on XInput
    enum class GamepadButton
    {
        DPadUp,
        DPadDown,
        DPadLeft,
        DPadRight,
        ButtonA,
        ButtonB,
        ButtonX,
        ButtonY,
        LeftShoulder,
        RightShoulder,
        LeftThumb,
        RightThumb,
        Start,
        Back
    };

    class Controller
    {
    public:
        Controller(int controllerIndex);
        ~Controller();
        Controller(const Controller&) = delete;
        Controller& operator=(const Controller&) = delete;
        Controller(Controller&&) noexcept;
        Controller& operator=(Controller&&) noexcept;

        void Update();
        bool IsButtonPressed(GamepadButton button) const;
        bool IsButtonReleased(GamepadButton button) const;
        bool IsButtonDown(GamepadButton button) const;
        int GetIndex() const { return m_ControllerIndex; }
        BYTE GetRightTriggerValue() const;
        BYTE GetLeftTriggerValue() const;
    private:
        class ControllerImpl;
        ControllerImpl* m_pImpl;
        int m_ControllerIndex;
    };
}
