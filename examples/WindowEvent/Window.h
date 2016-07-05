#pragma once

class WindowEvent;    // Forward Declaration of event class if necessary

class Window : public tSubject<const WindowEvent&>
{
public:
    Window();
    virtual ~Window();

    void testNotifications();
};

