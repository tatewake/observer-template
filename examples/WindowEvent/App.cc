#include "tObserver.h"
#include "App.h"
#include "WindowEvent.h"

App::App()
{

}

App::~App()
{

}

void App::update(const WindowEvent& message)
{
    switch(message.mEvent)
    {
        case WindowEvent::kOpened:
            printf("*** The window opened!\n");
            break;

        case WindowEvent::kClosed:
            printf("*** The window closed!\n");
            break;

        case WindowEvent::kResized:
            printf("*** The window resized to: (%lu, %lu)\n", message.mWidth, message.mHeight);
            break;

        case WindowEvent::kFocusChanged:
            printf("*** The window's focused change and now it... %s\n",
                   (message.mHasFocus) ? "has focus" : "lost focus");
            break;

        default:
            printf("*** %s\n", "Got a window message I couldn't handle!");
            break;
    }
}
