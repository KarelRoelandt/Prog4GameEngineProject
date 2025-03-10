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

    // Enhanced enum for stick directions to support diagonal movements
    enum class StickDirection
    {
        None,
        Left,
        Right,
        Up,
        Down,
        UpLeft,
        UpRight,
        DownLeft,
        DownRight
    };

    // Enum for stick types
    enum class StickType
    {
        LeftStick,
        RightStick
    };

    // Stick state struct to store full stick information
    struct StickState
    {
        float x = 0.0f;
        float y = 0.0f;
        float magnitude = 0.0f;
        float angle = 0.0f;
        StickDirection direction = StickDirection::None;
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

        // Trigger methods
        BYTE GetRightTriggerValue() const;
        BYTE GetLeftTriggerValue() const;

        // New analog stick methods
        float GetLeftStickX() const;
        float GetLeftStickY() const;
        float GetRightStickX() const;
        float GetRightStickY() const;

    private:
        class ControllerImpl;
        ControllerImpl* m_pImpl;
        int m_ControllerIndex;
    };
}