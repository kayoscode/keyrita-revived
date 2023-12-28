#pragma once

#include "Window.h"

namespace wgui
{
   class Application
   {
   public:
      static void AddWindow(wgui::WindowBase* window, GLFWwindow* gWin)
      {
         mWindows[gWin] = window;
      }

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

      static DebugLogger Logger;

   private:
      static std::map<GLFWwindow*, wgui::WindowBase*> mWindows;
   };
}
