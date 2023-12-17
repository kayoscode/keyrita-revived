#include <iostream>
#include "Window.h" 

using namespace wgui;
int main()
{
    MainWindow mainWindow;
    mainWindow.CreateWindow("Keyrita", 800, 600, false, true, true, false);

    while (!mainWindow.Closing())
    {
        mainWindow.NewFrame();
    }

    return 0;
}