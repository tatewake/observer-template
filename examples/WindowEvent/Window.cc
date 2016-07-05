#include "tObserver.h"
#include "Window.h"
#include "WindowEvent.h"

Window::Window()
{

}

Window::~Window()
{

}

void Window::testNotifications()
{
    notify(WindowEvent(WindowEvent::kOpened, this));
    notify(WindowEvent::kClosed);
    notify(WindowEvent(WindowEvent::kResized, this, 128, 96));
    notify(WindowEvent(WindowEvent::kFocusChanged, NULL, true));
    notify(WindowEvent(WindowEvent::kFocusChanged, NULL, false));
    notify(WindowEvent::kMinimized);
}

