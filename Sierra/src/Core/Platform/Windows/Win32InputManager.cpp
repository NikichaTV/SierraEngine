//
// Created by Nikolay Kanchevski on 10.17.2023.
//

#include "Win32InputManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Win32InputManager::Win32InputManager(const InputManagerCreateInfo &createInfo)
        : InputManager(createInfo)
    {

    }

    /* --- GETTER METHODS --- */

    bool Win32InputManager::IsKeyPressed(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool Win32InputManager::IsKeyHeld(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool Win32InputManager::IsKeyReleased(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool Win32InputManager::IsKeyResting(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool Win32InputManager::IsMouseButtonPressed(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool Win32InputManager::IsMouseButtonHeld(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool Win32InputManager::IsMouseButtonReleased(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    bool Win32InputManager::IsMouseButtonResting(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    Vector2 Win32InputManager::GetMouseScroll() const
    {
        return mouseScroll;
    }

    /* --- PRIVATE METHODS --- */

    void Win32InputManager::Update()
    {
        // Swap out current key/mouse states and move them to the array for the last frame
        std::copy(keyStates.begin(), keyStates.end(), lastKeyStates.begin());
        std::copy(mouseButtonStates.begin(), mouseButtonStates.end(), lastMouseButtonStates.begin());
        mouseScroll = { 0, 0 };
    }

    /* --- EVENTS --- */

    void Win32InputManager::KeyMessage(const UINT message, const WPARAM wParam, const LPARAM lParam)
    {
        // If key has been filtered out we return
        if (wParam == VK_PROCESSKEY) return;

        // Store key code and, if invalid, try to map it to a virtual key code
        uint32 keyCode = wParam != 0 ? static_cast<uint32>(wParam) : MapVirtualKeyW(static_cast<uint32>(wParam), MAPVK_VK_TO_VSC);

        // Prevent out of bounds error
        if (keyCode >= (sizeof(KEY_TABLE) / sizeof(KEY_TABLE[0]))) return;

        // Translate key
        Key key = KEY_TABLE[keyCode];
        if (key == Key::Unknown) return;

        // Distinguish double keys, as they share the same initial code
        if (HIWORD(lParam) & KF_EXTENDED)
        {
            switch (key)
            {
                case Key::LeftControl:
                {
                    key = Key::RightControl;
                    break;
                }
                case Key::LeftAlt:
                {
                    key = Key::RightAlt;
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        // Since the legacy AltGr key is not handled as a separate key, but rather as a Left Control followed by Right Alt
        // event,we must capture the second event to not detect 2 physical presses, when a just single one has happened
        if (wParam == VK_CONTROL)
        {
            MSG temporaryMessage;
            const DWORD time = GetMessageTime();
            if (PeekMessageW(&temporaryMessage, nullptr, 0, 0, PM_NOREMOVE))
            {
                switch (temporaryMessage.message)
                {
                    case WM_KEYDOWN:
                    case WM_SYSKEYDOWN:
                    case WM_KEYUP:
                    case WM_SYSKEYUP:
                    {
                        if (temporaryMessage.wParam == VK_MENU && (HIWORD(temporaryMessage.lParam) & KF_EXTENDED) && temporaryMessage.time == time)
                        {
                            // This is the Left Control message, so we exit before Right Alt gets the chance
                            return;
                        }
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
                if (
                    (temporaryMessage.message == WM_KEYDOWN || temporaryMessage.message == WM_SYSKEYDOWN || temporaryMessage.message == WM_KEYUP || temporaryMessage.message == WM_SYSKEYUP) &&
                    (temporaryMessage.wParam == VK_MENU && (HIWORD(temporaryMessage.lParam) & KF_EXTENDED) && temporaryMessage.time == time)
                )
                {
                    return;
                }
            }
        }

        // Explicitly handle PrintScreen, as it only sends out KEY_RELEASE messages
        if (key == Key::PrintScreen)
        {
            // Save key states
            lastKeyStates[GetKeyIndex(key)] = InputAction::Press;
            keyStates[GetKeyIndex(key)] = InputAction::Release;

            // Trigger events
            GetKeyPressDispatcher().DispatchEvent(key);
            GetKeyReleaseDispatcher().DispatchEvent(key);
            return;
        }
        // Also define custom logic, as both Shift keys are reported as single VK_SHIFT message, and we need to distinguish them
        else if (key == Key::LeftShift)
        {
            if (message == WM_KEYDOWN)
            {
                if (GetKeyState(VK_LSHIFT) & KF_UP)
                {
                    keyStates[GetKeyIndex(Key::LeftShift)] = InputAction::Press;
                    GetKeyPressDispatcher().DispatchEvent(Key::LeftShift);
                    return;
                }
                else if (GetKeyState(VK_RSHIFT) & KF_UP)
                {
                    keyStates[GetKeyIndex(Key::RightShift)] = InputAction::Press;
                    GetKeyPressDispatcher().DispatchEvent(Key::RightShift);
                    return;
                }
            }
            else if (message == WM_KEYUP)
            {
                if (IsKeyPressed(Key::LeftShift) || IsKeyHeld(Key::LeftShift) && !(GetKeyState(VK_LSHIFT) & KF_UP))
                {
                    keyStates[GetKeyIndex(Key::LeftShift)] = InputAction::Release;
                    GetKeyPressDispatcher().DispatchEvent(Key::LeftShift);
                    return;
                }
                if (!IsKeyPressed(Key::LeftShift) && !IsKeyHeld(Key::LeftShift) && !(GetKeyState(VK_RSHIFT) & KF_UP))
                {
                    keyStates[GetKeyIndex(Key::RightShift)] = InputAction::Release;
                    GetKeyReleaseDispatcher().DispatchEvent(Key::RightShift);
                    return;
                }
            }
        }

        // Get key action
        const InputAction action = HIWORD(lParam) & KF_UP ? InputAction::Release : InputAction::Press;

        // Save key state and trigger events
        keyStates[GetKeyIndex(key)] = action;
        action == InputAction::Press ? GetKeyPressDispatcher().DispatchEvent(key) : GetKeyReleaseDispatcher().DispatchEvent(key);
    }

    void Win32InputManager::MouseButtonMessage(const UINT message, const WPARAM wParam, const LPARAM)
    {
        switch (message)
        {
            case WM_LBUTTONDOWN:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Left;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
                GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
                break;
            }
            case WM_LBUTTONUP:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Left;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
                GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
                break;
            }
            case WM_RBUTTONDOWN:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Right;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
                GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
                break;
            }
            case WM_RBUTTONUP:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Right;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
                GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
                break;
            }
            case WM_MBUTTONDOWN:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Middle;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
                GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
                break;
            }
            case WM_MBUTTONUP:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Middle;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
                GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
                break;
            }
            case WM_XBUTTONDOWN:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? MouseButton::Extra1 : MouseButton::Extra2;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
                GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
                break;
            }
            case WM_XBUTTONUP:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? MouseButton::Extra1 : MouseButton::Extra2;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
                GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    void Win32InputManager::MouseWheelMessage(const UINT message, const WPARAM wParam, const LPARAM)
    {
        // We apply division, so scrolling feels similar to that of the rest of the platforms
        if (message == WM_MOUSEWHEEL) mouseScroll.y = static_cast<float32>(static_cast<SHORT>(HIWORD(wParam))) / WHEEL_DELTA;
        else if (message == WM_MOUSEHWHEEL) mouseScroll.x = -static_cast<float32>(static_cast<SHORT>(HIWORD(wParam))) / WHEEL_DELTA;

        // Dispatch events
        GetMouseScrollDispatcher().DispatchEvent(mouseScroll);
    }

}