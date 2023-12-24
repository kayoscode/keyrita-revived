#pragma once
#include "DebugLogger.h"
#include "ContextManager.h"
#include <thread>

#include "include_nuk.h"

class GLFWwindow;
class GLFWmonitor;

namespace wgui
{
   class WindowRenderer;

   /// <summary>
   /// Abstract class for handling basic operations a window can do.
   /// Used for the main window and pop up dialogs.
   /// TODO: handle refresh rate?
   /// </summary>
   class WindowBase
   {
   public:
      WindowBase();
      virtual ~WindowBase();

      void SetWindowSize(int width, int height);
      void GetWindowSize(int& width, int& height) const;
      void SetWindowPos(int x, int y);
      void GetWindowPos(int& x, int& y) const;
      void SetWindowSizeLimits(int minWidth, int minHeight, int maxWidth, int maxHeight);

      void SetWindowTitle(const std::string& title);
      std::string GetWindowTitle() const { return mWindowTitle; }

      /// <summary>
      /// Getters for window attributes.
      /// </summary>
      bool GetWindowFocused() const;
      bool GetWindowVisible() const;
      bool GetWindowResizable() const;
      bool GetWindowDecorated() const;

      void CloseWindow();
      void CancelWindowClose();
      bool Closing() const;

      virtual bool CreateWindow(const std::string& title,
         int width, int height,
         bool resizable = true,
         bool visible = true, bool decorated = true, bool fullScreen = false) = 0;

      /// <summary>
      /// Renders the next frame to the screen.
      /// </summary>
      virtual void Render() = 0;
      virtual void Update();

      NuklearGlfwContextManager& GetGlfwContext() { return mNkContext; }

      void SetRenderer(WindowRenderer* renderer) { mLastRenderer = renderer; }
      WindowRenderer* GetRenderer() const { return mLastRenderer; }

   protected:
      WindowRenderer* mLastRenderer = nullptr;

      GLFWwindow* mWindow;
      std::string mWindowTitle;

      NuklearGlfwContextManager mNkContext;

      friend class WindowInput;
   };

   /// <summary>
   /// Class handling the main window.
   /// Includes a logger for diagnostics.
   /// </summary>
   class MainWindow : public WindowBase
   {
   public:
      MainWindow();

      bool CreateWindow(const std::string& title,
         int width, int height,
         bool resizable = true,
         bool visible = true, bool decorated = true, bool fullScreen = false) override;

      void Render() override;

   private:
      static DebugLogger GlfwLogger;
      static DebugLogger GlLogger;
   };

   //TODO:
   //class Dialog : public WindowBase

   /// <summary>
   /// Class responsible for providing callers with input from windows.
   /// Once initialized, the window must stay alive for this class to remain valid.
   /// </summary>
   class WindowInput
   {
   public:
      /// <summary>
      /// Give a pointer to the window base. 
      /// This class doesn't own the window, but we trust it will stick around for as long as 
      /// the user calls functions from this class.
      /// </summary>
      /// <param name="window"></param>
      WindowInput(const WindowBase* window)
         : mWindow(window) { }

      /// <summary>
      /// Mouse and keyboard events.
      /// </summary>
      /// <param name="key"></param>
      /// <returns></returns>
      bool IsKeyDown(nk_keys key) const;
      bool IsKeyClicked(nk_keys key) const;
      void GetKeyboardText(std::string& resultString) const;

      bool IsMouseButtonDown(nk_buttons button) const;
      bool IsMouseButtonClicked(nk_buttons button) const;
      void GetMouseDelta(int& deltaX, int& deltaY) const;
      void GetMousePos(int& posX, int& posY) const;
      void GetMousePrevPos(int& posX, int& posY) const;
      void GetMouseScrollDelta(int& deltaX, int& deltaY) const;

   private:
      const WindowBase* const mWindow;
   };

   static class Application
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

      static void Start(MainWindow* mainWindow);

      //static void CreateDialog(wgui::WindowBase* window, GLFWwindow* gwin);
      //static void CloseDialog(wgui::WindowBase* window);

   private:
      static std::map<GLFWwindow*, wgui::WindowBase*> mWindows;
      static std::thread mMainWindowRenderThread;
   };
}