#include <iostream>
#include "Window.h" 

using namespace wgui;
int main()
{
    MainWindow mainWindow;
    mainWindow.CreateWindow("Keyrita", 1920, 1080, false, true, true, false);
    mainWindow.SetWindowSizeLimits(800, 600, -1, -1);

    while (!mainWindow.Closing())
    {
        mainWindow.NewFrame();
    }

    return 0;
}