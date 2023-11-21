//
// Created by Nikolay Kanchevski on 17.08.23.
//

#pragma once

#if !SR_PLATFORM_MACOS
    #error "Including the CocoaWindow.h file is only allowed in macOS builds!"
#endif

#include "../../Window.h"
#include "CocoaContext.h"
#include "CocoaInputManager.h"
#include "CocoaCursorManager.h"

#if !defined(__OBJC__)
    namespace Sierra
    {
        typedef void CocoaWindowDelegate;
        typedef void CocoaWindowContentView;
        typedef void CocoaWindowImplementation;
    }
#else
    #include <Cocoa/Cocoa.h>
    @class CocoaWindowDelegate;
    @class CocoaWindowContentView;
    @class CocoaWindowImplementation;
#endif

namespace Sierra
{

    class SIERRA_API CocoaWindow final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit CocoaWindow(const CocoaContext &cocoaContext, const WindowCreateInfo &createInfo);

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
        [[nodiscard]] std::string GetTitle() const override;
        [[nodiscard]] Vector2Int GetPosition() const override;
        [[nodiscard]] Vector2UInt GetSize() const override;
        [[nodiscard]] Vector2UInt GetFramebufferSize() const override;
        [[nodiscard]] float32 GetOpacity() const override;
        [[nodiscard]] bool IsClosed() const override;
        [[nodiscard]] bool IsMinimized() const override;
        [[nodiscard]] bool IsMaximized() const override;
        [[nodiscard]] bool IsFocused() const override;
        [[nodiscard]] bool IsHidden() const override;

        [[nodiscard]] Screen& GetScreen() override;
        [[nodiscard]] InputManager& GetInputManager() override;
        [[nodiscard]] CursorManager& GetCursorManager() override;
        [[nodiscard]] WindowAPI GetAPI() const override;

        /* --- EVENTS --- */
        #if defined(__OBJC__)
            void WindowShouldClose();
            void WindowDidResize(const NSNotification* notification);
            void WindowDidMove(const NSNotification* notification);
            void WindowDidMiniaturize(const NSNotification* notification);
            void WindowDidBecomeKey(const NSNotification* notification);
            void WindowDidResignKey(const NSNotification* notification);
            void WindowDidChangeScreen(const NSNotification* notification);
        #endif

        /* --- DESTRUCTOR --- */
        ~CocoaWindow();

    private:
        const CocoaContext &cocoaContext;

        NSWindow* window = nullptr;
        CocoaWindowDelegate* delegate = nullptr;
        CocoaWindowContentView* view = nullptr;

        CocoaScreen* screen = nullptr;
        CocoaInputManager inputManager;
        CocoaCursorManager cursorManager;

        bool maximized = false;
        bool closed = false;

        float32 GetTitleBarHeight() const;

    };

}