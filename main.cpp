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

   WindowRendererGui renderer;

   GuiWindow primaryWindow("Keyrita Main", 0, 0, 1920, 1080);
   GuiLayoutRowDynamic layout(40);
   GuiLabel label("Here is a basic label!", eTextAlignmentFlags::FullyCentered);
   layout.AddControl(&label);
   layout.AddControl(&label);
   layout.AddControl(&label);

   primaryWindow.AddControl(&layout);
   renderer.AddWindow(&primaryWindow);

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