#include <iostream>
#include "Window.h"
#include "NuklearWindowRenderer.h"

using namespace wgui;

int main()
{
   MainWindow mainWindow;
   mainWindow.CreateWindow("Keyrita", 1600, 1200, false, true, true, false);
   mainWindow.SetWindowSizeLimits(1200, 900, -1, -1);

   XmlLoadedWindowRenderer renderer;
   renderer.ConstructLayoutFromXmlFile("./res/gui/Keyrita.guix");

   Timer t;
   int frameCount = 0;
   while (!mainWindow.Closing())
   {
      mainWindow.NewFrame(&renderer);

      if (t.milliseconds() >= 5000)
      {
         std::cout << "Fps: " << frameCount / 5 << "\n";
         frameCount = 0;
         t.reset();
      }

      frameCount++;
   }

   return 0;
}