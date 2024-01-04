//
// Created by Nikolay Kanchevski on 8.10.23.
//

#pragma once

#include "Event.h"
#include "../Core/Touch.h"

namespace Sierra
{

    class SIERRA_API TouchEvent : public Event
    {
    public:
        [[nodiscard]] inline const Touch& GetTouch() const { return touch; }

    protected:
        TouchEvent(const Touch &touch) : touch(touch) { }

    private:
        Touch touch;

    };

    class SIERRA_API TouchBeginEvent : public TouchEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TouchBeginEvent(const Touch &touch) : TouchEvent(touch) { }

    };

    class SIERRA_API TouchMoveEvent : public TouchEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TouchMoveEvent(const Touch &touch) : TouchEvent(touch) { }

    };

    class SIERRA_API TouchEndEvent : public TouchEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TouchEndEvent(const Touch &touch) : TouchEvent(touch) { }

    };

}
