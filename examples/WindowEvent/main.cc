#include "tObserver.h"
#include "App.h"
#include "Window.h"

App* gApp = NULL;
Window* gWindow = NULL;

void someStartupMethod()
{
    gApp = new App();
    gWindow = new Window();

    gWindow->attach(gApp);
}

void someShutdownMethod()
{
    if (gApp)       { delete gApp;      gApp = NULL; }
    if (gWindow)    { delete gWindow;   gWindow = NULL; }
}

int main(int argc, char *argv[])
{
    someStartupMethod();

    if (gWindow)
    {
        gWindow->testNotifications();
    }

    someShutdownMethod();

    return 0;
}