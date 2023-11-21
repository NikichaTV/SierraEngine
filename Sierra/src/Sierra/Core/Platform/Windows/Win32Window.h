//
// Created by Nikolay Kanchevski on 8.21.2023.
//
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the Win32Window.h file is only allowed in Windows builds!"
#endif

#include "../../Window.h"
#include "WindowsInstance.h"
#include "Win32InputManager.h"
#include "Win32CursorManager.h"

namespace Sierra
{

    class SIERRA_API Win32Window final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Win32Window(const WindowCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void OnUpdate() override;
        void Minimize() override;
        void Maximize() override;
        void Show() override;
        void Hide() override;
        void Focus() override;
        void Close() override;

        /* --- SETTER METHODS --- */
        void SetTitle(const std::string &title) override;
        void SetPosition(const Vector2Int &position) override;
        void SetSize(const Vector2UInt &size) override;
        void SetOpacity(float32 opacity) override;

        /* --- GETTER METHODS --- */
        std::string GetTitle() const override;
        Vector2Int GetPosition() const override;
        Vector2UInt GetSize() const override;
        Vector2UInt GetFramebufferSize() const override;
        float32 GetOpacity() const override;
        bool IsClosed() const override;
        bool IsMinimized() const override;
        bool IsMaximized() const override;
        bool IsFocused() const override;
        bool IsHidden() const override;

        InputManager& GetInputManager() override;
        CursorManager& GetCursorManager() override;
        WindowAPI GetAPI() const override;

        /* --- DESTRUCTOR --- */
        ~Win32Window() override;

    private:
        WindowsInstance &windowsInstance;
        HWND window;

        Win32InputManager inputManager;
        Win32CursorManager cursorManager;

        std::string title;
        bool closed = false;

        bool justBecameShown = false; // This is to prevent all the made up events the window manager sends when showing a hidden window
        bool nextMoveEventBlocked = false; // This is to prevent detecting when window manager moves window to [x: ~-32000, y: ~-32000] when hiding/minimizing it

        void AdjustWindowSizeForDPI(RECT &rect);
        static uint16 GetTitleBarHeight() { return GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYBORDER) + GetSystemMetrics(SM_CYFRAME); }
        static LRESULT CALLBACK WindowProc(HWND callingWindow, UINT message, WPARAM wParam, LPARAM lParam);

    };

}
