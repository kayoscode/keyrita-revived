#include "NuklearWindowRenderer.h"
#include "OperatingSystem.h"
#include "PlatformModules.h"
#include "App.h"
#include "XmlToUi.h"
#include "KeyritaControls.h"

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
   XmlToUiUtil::Init();
   XmlToUiUtil::AddControlFactory<KeyritaControlsFactory>();

   MainWindow mainWindow;
   mainWindow.CreateWindow("Keyrita", 1600, 1200, false, true, true, false);
   mainWindow.SetWindowSizeLimits(1200, 900);

   //MainWindow secondWindow;
   //secondWindow.CreateWindow("Dialog", 400, 300, false, true, true, false);

   bool resiable = false;

   XmlRenderer mainWindowRenderer;
   mainWindowRenderer.ConstructLayoutFromXmlFile("./res/gui/Keyrita.guix");
   mainWindowRenderer.Init();
   mainWindow.SetRenderer(&mainWindowRenderer);

   //XmlRenderer secondRenderer;
   //secondRenderer.ConstructLayoutFromXmlFile("./res/gui/Keyrita.guix");
   //secondRenderer.Init();
   //secondWindow.SetRenderer(&mainWindowRenderer);

   Timer t;
   int frameCount = 0;
   while (!mainWindow.Closing())
   {
      Application::RenderWindows();

      if (t.milliseconds() >= 5000)
      {
         std::cout << "Fps: " << frameCount / 5 << "\n";
         frameCount = 0;
         t.reset();
      }

      frameCount++;
   }

   Application::Shutdown();

   return 0;
}