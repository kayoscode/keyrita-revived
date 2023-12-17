#pragma once
#include "DebugLogger.h"
#include "ContextManager.h"

class GLFWwindow;
class GLFWmonitor;

namespace wgui 
{
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
        void GetWindowSize(int& width, int& height);
        void SetWindowPos(int x, int y);
        void GetWindowPos(int& x, int& y);
        void SetWindowSizeLimits(int minWidth, int minHeight, int maxWidth, int maxHeight);

        void SetWindowTitle(const std::string& title);
        std::string GetWindowTitle() { return mWindowTitle; }

        /// <summary>
        /// Getters for window attributes.
        /// </summary>
        bool GetWindowFocused();
        bool GetWindowVisible();
        bool GetWindowResizable();
        bool GetWindowDecorated();

        void CloseWindow();
        void CancelWindowClose();
        bool Closing();

        virtual bool CreateWindow(const std::string& title, 
            int width, int height,
            bool resizable = true,
            bool visible = true, bool decorated = true, bool fullScreen = false) = 0;

        /// <summary>
        /// Renders the next frame to the screen.
        /// </summary>
        virtual void NewFrame() = 0;

    protected:
        static constexpr int MaxVertexBuffer = 512 * 1024;
        static constexpr int MaxElementBuffer = 128 * 1024;

        GLFWwindow* mWindow;
        std::string mWindowTitle;
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

        void NewFrame() override;

    private:
        static DebugLogger GlfwLogger;
        static DebugLogger GlLogger;
        NuklearGlfwContextManager mNkContext;
    };

    //TODO:
    //class Dialog : public WindowBase
}