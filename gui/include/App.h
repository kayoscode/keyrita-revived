#pragma once

#include "Window.h"

namespace wgui
{
   class Application
   {
   public:
      static void AddMainWindow(wgui::MainWindow* mainWindow)
      {
         AddWindow(mainWindow);
         mMainWindow = mainWindow;
      }

      static void AddDialog(wgui::Dialog* dialog)
      {
         AddWindow(dialog);
      }

      static void CloseDialog(wgui::Dialog* dialog)
      {
         dialog->CloseWindow();
         mWindows.erase(mWindows.find(dialog->GetContext().GetGlfw()->win));
      }

      static MainWindow* GetMainWindow()
      {
         return mMainWindow;
      }

      static void RenderWindows();

      /// <summary>
      /// Returns window associated with the GLFWwindow* 
      /// nullptr if it doesn't exist.
      /// </summary>
      /// <param name="gWin"></param>
      /// <returns></returns>
      static wgui::WindowBase* GetWindow(GLFWwindow* gWin)
      {
         auto itt = mWindows.find(gWin);

         if (itt != mWindows.end())
         {
            return itt->second;
         }

         return nullptr;
      }

      static void Start()
      {
         Logger.setPrefix("\\[[pn]\\]: ");
         Logger.setLevel(Level::LEVEL_TRACE);
      }

      static void Shutdown();

      static DebugLogger Logger;

   private:
      static void AddWindow(wgui::WindowBase* window)
      {
         mWindows[window->GetContext().GetGlfw()->win] = window;
      }

      static std::map<GLFWwindow*, wgui::WindowBase*> mWindows;
      static MainWindow* mMainWindow;
   };
}
