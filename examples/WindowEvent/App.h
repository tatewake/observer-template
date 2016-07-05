#pragma once

class WindowEvent;    // Forward Declaration of event class if necessary

class App : public tObserver<const WindowEvent&>
{
public:
    App();
    virtual ~App();

public:
    virtual void update(const WindowEvent& message);
};

