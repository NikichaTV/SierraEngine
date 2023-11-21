//
// Created by Nikolay Kanchevski on 15.08.23.
//

#pragma once

#include "Event.h"

namespace Sierra
{

    class SIERRA_API WindowEvent : public Event
    {

    };

    class SIERRA_API WindowResizeEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit WindowResizeEvent(const Vector2UInt size) : size(size) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const Vector2UInt GetSize() const { return size; }

        /* --- PROPERTIES --- */
        #if SR_ENABLE_LOGGING
            inline std::string ToString() const override { return (std::stringstream() << "Window Resized - [Width: " << size.x << " | Height: " << size.y << "]").str(); }
        #endif

    private:
        Vector2UInt size;

    };

    class SIERRA_API WindowCloseEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        WindowCloseEvent() = default;

        /* --- PROPERTIES --- */
        #if SR_ENABLE_LOGGING
            inline std::string ToString() const override { return (std::stringstream() << "Window Closed").str(); }
        #endif

    };

    class SIERRA_API WindowMoveEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit WindowMoveEvent(const Vector2Int &position) : position(position) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] Vector2Int GetPosition() const { return position; }

        /* --- PROPERTIES --- */
        #if SR_ENABLE_LOGGING
            inline std::string ToString() const override { return (std::stringstream() << "Window Moved - [X: " << position.x << " | Y: " << position.y << "]").str(); }
        #endif

    private:
        Vector2Int position;

    };

    class SIERRA_API WindowFocusEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit WindowFocusEvent(const bool focused) : focused(focused) { };

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsFocused() const { return focused; }

        /* --- PROPERTIES --- */
        #if SR_ENABLE_LOGGING
            inline std::string ToString() const override { return (std::stringstream() << "Window " << (focused ? "Focused" : "Unfocused")).str(); }
        #endif

    private:
        bool focused;

    };

    class SIERRA_API WindowMinimizeEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        WindowMinimizeEvent() = default;

        /* --- PROPERTIES --- */
        #if SR_ENABLE_LOGGING
            inline std::string ToString() const override { return (std::stringstream() << "Window Minimized").str(); }
        #endif

    };

    class SIERRA_API WindowMaximizeEvent final : public WindowEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        WindowMaximizeEvent() = default;

        /* --- PROPERTIES --- */
        #if SR_ENABLE_LOGGING
            inline std::string ToString() const override { return (std::stringstream() << "Window Maximized").str(); }
        #endif

    };

}
