#pragma once
#include "ContextManager.h"
#include "DebugLogger.h"
#include <thread>

#include "include_nuk.h"

class GLFWwindow;
class GLFWmonitor;

namespace wgui
{
class WindowStyle
{
 public:
   WindowStyle(nk_style *copyFrom, int fontSize = 16) : mFontSize(fontSize)
   {
      std::memcpy(&mStyle, copyFrom, sizeof(nk_style));
   }

   WindowStyle(const WindowStyle &copy) : mFontSize(copy.mFontSize)
   {
      std::memcpy(&mStyle, &copy.mStyle, sizeof(nk_style));
   }

   int GetFontSize()
   {
      return mFontSize;
   }
   void SetFontSize(int fontSize)
   {
      mFontSize = fontSize;
   }

   void Scale(nk_style *style, nk_font *font, float scaleX, float scaleY);

 private:
   nk_style mStyle;
   int mFontSize;
};

class WindowRenderer;
class WindowBase;

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
   WindowInput(const WindowBase *window) : mWindow(window)
   {
   }

   /// <summary>
   /// Mouse and keyboard events.
   /// </summary>
   /// <param name="key"></param>
   /// <returns></returns>
   bool IsKeyDown(nk_keys key) const;
   bool IsKeyClicked(nk_keys key) const;
   void GetKeyboardText(std::string &resultString) const;

   bool IsMouseButtonDown(nk_buttons button) const;
   bool IsMouseButtonClicked(nk_buttons button) const;
   void GetMouseDelta(int &deltaX, int &deltaY) const;
   void GetMousePos(int &posX, int &posY) const;
   void GetMousePrevPos(int &posX, int &posY) const;
   void GetMouseScrollDelta(int &deltaX, int &deltaY) const;

 private:
   const WindowBase *const mWindow;
};

/// <summary>
/// Abstract class for handling basic operations a window can do.
/// Used for the main window and pop up dialogs.
/// </summary>
class WindowBase
{
 public:
   WindowBase();
   virtual ~WindowBase();

   void SetWindowSize(int width, int height);
   void GetWindowSize(int &width, int &height) const;
   void SetWindowPos(int x, int y);
   void GetWindowPos(int &x, int &y) const;
   void SetWindowSizeLimits(int minWidth, int minHeight, int maxWidth = GL_DONT_CARE, int = GL_DONT_CARE);

   void SetWindowTitle(const std::string &title);
   std::string GetWindowTitle() const
   {
      return mWindowTitle;
   }

   /// <summary>
   /// Getters for window attributes.
   /// </summary>
   bool GetWindowFocused() const;
   bool GetWindowVisible() const;
   bool GetWindowResizable() const;
   bool GetWindowDecorated() const;

   void CloseWindow();
   void CancelWindowClose();
   bool Closing()
   {
      return mClosing;
   }

   virtual bool CreateWindow(const std::string &title, int width, int height, bool resizable = true,
                             bool visible = true, bool decorated = true, bool fullScreen = false) = 0;

   /// <summary>
   /// Renders the next frame to the screen.
   /// </summary>
   virtual void Render();
   virtual void Update();

   virtual NuklearGlfwContextManager &GetContext()
   {
      return mNkContext;
   }

   void SetRenderer(WindowRenderer *renderer)
   {
      mLastRenderer = renderer;
   }
   WindowRenderer *GetRenderer() const
   {
      return mLastRenderer;
   }

   void SetContentScale();
   double GetContentScaleX()
   {
      return mContentScaleX;
   }
   double GetContentScaleY()
   {
      return mContentScaleY;
   }

   virtual WindowStyle *GetStyle()
   {
      return mWindowStyle.get();
   }
   WindowInput &GetInput()
   {
      return mWindowInput;
   }

 protected:
   WindowRenderer *mLastRenderer = nullptr;

   WindowInput mWindowInput;
   GLFWwindow *mWindow;
   std::string mWindowTitle;
   double mContentScaleX = 0.0;
   double mContentScaleY = 0.0;
   struct nk_font *mFont;
   bool mClosing = false;

   NuklearGlfwContextManager mNkContext;
   std::unique_ptr<WindowStyle> mWindowStyle;

   friend class WindowInput;
   friend class Application;
   friend class Dialog;
};

/// <summary>
/// Class handling the main window.
/// Includes a logger for diagnostics.
/// </summary>
class MainWindow : public WindowBase
{
 public:
   MainWindow();

   bool CreateWindow(const std::string &title, int width, int height, bool resizable = true, bool visible = true,
                     bool decorated = true, bool fullScreen = false) override;

 private:
   static DebugLogger GlfwLogger;
   static DebugLogger GlLogger;
};

/// <summary>
/// Creates a window assuming
/// </summary>
class Dialog : public WindowBase
{
 public:
   Dialog(MainWindow *mainWindow) : mMainWindow(mainWindow)
   {
   }

   bool CreateWindow(const std::string &title, int width, int height, bool resizable = true, bool visible = true,
                     bool decorated = true, bool fullScreen = false) override;

 private:
   // A non-owned pointer to the main window.
   MainWindow *mMainWindow;
};
} // namespace wgui