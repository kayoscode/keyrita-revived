#pragma once
#include "DebugLogger.h"
#include "ContextManager.h"

class GLFWwindow;
class GLFWmonitor;
enum nk_keys;
enum nk_buttons;

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
        virtual void NewFrame() = 0;

    protected:
        static constexpr int MaxVertexBuffer = 512 * 1024;
        static constexpr int MaxElementBuffer = 128 * 1024;

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

        void NewFrame() override;

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
        WindowInput(const WindowBase const* window)
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
}