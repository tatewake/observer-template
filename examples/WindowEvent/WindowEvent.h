#pragma once

class Window;

class WindowEvent
{
public:
    enum EventType
    {
        kOpened = 0,
        kClosed,
        kResized,
        kFocusChanged,
        kMinimized,
    };

public:
    Window*   mSource;
    EventType mEvent;
    size_t    mWidth, mHeight;
    bool      mHasFocus;

public:
    WindowEvent(const EventType& newEventType, Window* newSource = NULL, bool newHasFocus = false)
    : mSource(newSource), mEvent(newEventType), mWidth(0), mHeight(0), mHasFocus(newHasFocus) { }
    WindowEvent(const EventType& newEventType, Window* newSource, const size_t& newWidth, const size_t& newHeight)
    : mSource(newSource), mEvent(newEventType), mWidth(newWidth), mHeight(newHeight), mHasFocus(false) { }
};
