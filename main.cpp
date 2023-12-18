#include <iostream>
#include "Window.h"
#include "NuklearWindowRenderer.h"

#include "include_nuk.h"

using namespace wgui;

int main()
{
    MainWindow mainWindow;
    mainWindow.CreateWindow("Keyrita", 1920, 1080, false, true, true, false);
    mainWindow.SetWindowSizeLimits(800, 600, -1, -1);

    NuklearWindowRendererGui renderer;

    while (!mainWindow.Closing())
    {
        mainWindow.NewFrame(&renderer);
    }

    return 0;
}