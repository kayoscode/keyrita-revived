#include "NuklearWindowRenderer.h"
#include "OperatingSystem.h"
#include "PlatformModules.h"
#include "App.h"

using namespace wgui;

int main()
{
   Application::Start();

   std::unique_ptr<PlatformBase> platform;

   if (OperatingSystem::GetOperatingSystem() == eOsType::Windows)
   {
      platform = std::make_unique<PlatformWindows>();
   }
   else if (OperatingSystem::GetOperatingSystem() == eOsType::Linux)
   {
      platform = std::make_unique<PlatformLinux>();
   }
   platform->Initialize();

   MainWindow mainWindow;
   mainWindow.CreateWindow("Keyrita", 1600, 1200, false, true, true, false);
   mainWindow.SetWindowSizeLimits(1200, 900);

   bool resiable = false;

   XmlLoadedWindowRenderer renderer;
   renderer.ConstructLayoutFromXmlFile("./res/gui/Keyrita.guix");
   renderer.Init();

   mainWindow.SetRenderer(&renderer);

   Timer t;
   int frameCount = 0;
   while (!mainWindow.Closing())
   {
      mainWindow.Update();
      mainWindow.Render();

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