#include "NuklearWindowRenderer.h"
#include "Window.h"
#include "OperatingSystem.h"

#include "GL/glew.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT

#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

namespace 
{
	void GlfwErrorCallback(int errCode, const char *msg)
	{
		//mGlfwLogger.critical("{int}: {str}", errCode, msg);
	}

	enum class eTheme
	{
		Black,
		White,
		Red,
		Blue,
		Dark
	};

	static void SetStyle(struct nk_context *ctx, enum eTheme theme)
	{
		struct nk_color table[NK_COLOR_COUNT];
		if (theme == eTheme::White)
		{
			table[NK_COLOR_TEXT] = nk_rgba(70, 70, 70, 255);
			table[NK_COLOR_WINDOW] = nk_rgba(175, 175, 175, 255);
			table[NK_COLOR_HEADER] = nk_rgba(175, 175, 175, 255);
			table[NK_COLOR_BORDER] = nk_rgba(0, 0, 0, 255);
			table[NK_COLOR_BUTTON] = nk_rgba(185, 185, 185, 255);
			table[NK_COLOR_BUTTON_HOVER] = nk_rgba(170, 170, 170, 255);
			table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(160, 160, 160, 255);
			table[NK_COLOR_TOGGLE] = nk_rgba(150, 150, 150, 255);
			table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(120, 120, 120, 255);
			table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(175, 175, 175, 255);
			table[NK_COLOR_SELECT] = nk_rgba(190, 190, 190, 255);
			table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(175, 175, 175, 255);
			table[NK_COLOR_SLIDER] = nk_rgba(190, 190, 190, 255);
			table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(80, 80, 80, 255);
			table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(70, 70, 70, 255);
			table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(60, 60, 60, 255);
			table[NK_COLOR_PROPERTY] = nk_rgba(175, 175, 175, 255);
			table[NK_COLOR_EDIT] = nk_rgba(150, 150, 150, 255);
			table[NK_COLOR_EDIT_CURSOR] = nk_rgba(0, 0, 0, 255);
			table[NK_COLOR_COMBO] = nk_rgba(175, 175, 175, 255);
			table[NK_COLOR_CHART] = nk_rgba(160, 160, 160, 255);
			table[NK_COLOR_CHART_COLOR] = nk_rgba(45, 45, 45, 255);
			table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
			table[NK_COLOR_SCROLLBAR] = nk_rgba(180, 180, 180, 255);
			table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(140, 140, 140, 255);
			table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(150, 150, 150, 255);
			table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(160, 160, 160, 255);
			table[NK_COLOR_TAB_HEADER] = nk_rgba(180, 180, 180, 255);
			nk_style_from_table(ctx, table);
		}
		else if (theme == eTheme::Red)
		{
			table[NK_COLOR_TEXT] = nk_rgba(190, 190, 190, 255);
			table[NK_COLOR_WINDOW] = nk_rgba(30, 33, 40, 215);
			table[NK_COLOR_HEADER] = nk_rgba(181, 45, 69, 220);
			table[NK_COLOR_BORDER] = nk_rgba(51, 55, 67, 255);
			table[NK_COLOR_BUTTON] = nk_rgba(181, 45, 69, 255);
			table[NK_COLOR_BUTTON_HOVER] = nk_rgba(190, 50, 70, 255);
			table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(195, 55, 75, 255);
			table[NK_COLOR_TOGGLE] = nk_rgba(51, 55, 67, 255);
			table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 60, 60, 255);
			table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(181, 45, 69, 255);
			table[NK_COLOR_SELECT] = nk_rgba(51, 55, 67, 255);
			table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(181, 45, 69, 255);
			table[NK_COLOR_SLIDER] = nk_rgba(51, 55, 67, 255);
			table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(181, 45, 69, 255);
			table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(186, 50, 74, 255);
			table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(191, 55, 79, 255);
			table[NK_COLOR_PROPERTY] = nk_rgba(51, 55, 67, 255);
			table[NK_COLOR_EDIT] = nk_rgba(51, 55, 67, 225);
			table[NK_COLOR_EDIT_CURSOR] = nk_rgba(190, 190, 190, 255);
			table[NK_COLOR_COMBO] = nk_rgba(51, 55, 67, 255);
			table[NK_COLOR_CHART] = nk_rgba(51, 55, 67, 255);
			table[NK_COLOR_CHART_COLOR] = nk_rgba(170, 40, 60, 255);
			table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
			table[NK_COLOR_SCROLLBAR] = nk_rgba(30, 33, 40, 255);
			table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(64, 84, 95, 255);
			table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(70, 90, 100, 255);
			table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(75, 95, 105, 255);
			table[NK_COLOR_TAB_HEADER] = nk_rgba(181, 45, 69, 220);
			nk_style_from_table(ctx, table);
		}
		else if (theme == eTheme::Blue)
		{
			table[NK_COLOR_TEXT] = nk_rgba(20, 20, 20, 255);
			table[NK_COLOR_WINDOW] = nk_rgba(202, 212, 214, 215);
			table[NK_COLOR_HEADER] = nk_rgba(137, 182, 224, 220);
			table[NK_COLOR_BORDER] = nk_rgba(140, 159, 173, 255);
			table[NK_COLOR_BUTTON] = nk_rgba(137, 182, 224, 255);
			table[NK_COLOR_BUTTON_HOVER] = nk_rgba(142, 187, 229, 255);
			table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(147, 192, 234, 255);
			table[NK_COLOR_TOGGLE] = nk_rgba(177, 210, 210, 255);
			table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(182, 215, 215, 255);
			table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(137, 182, 224, 255);
			table[NK_COLOR_SELECT] = nk_rgba(177, 210, 210, 255);
			table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(137, 182, 224, 255);
			table[NK_COLOR_SLIDER] = nk_rgba(177, 210, 210, 255);
			table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(137, 182, 224, 245);
			table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(142, 188, 229, 255);
			table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(147, 193, 234, 255);
			table[NK_COLOR_PROPERTY] = nk_rgba(210, 210, 210, 255);
			table[NK_COLOR_EDIT] = nk_rgba(210, 210, 210, 225);
			table[NK_COLOR_EDIT_CURSOR] = nk_rgba(20, 20, 20, 255);
			table[NK_COLOR_COMBO] = nk_rgba(210, 210, 210, 255);
			table[NK_COLOR_CHART] = nk_rgba(210, 210, 210, 255);
			table[NK_COLOR_CHART_COLOR] = nk_rgba(137, 182, 224, 255);
			table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
			table[NK_COLOR_SCROLLBAR] = nk_rgba(190, 200, 200, 255);
			table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(64, 84, 95, 255);
			table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(70, 90, 100, 255);
			table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(75, 95, 105, 255);
			table[NK_COLOR_TAB_HEADER] = nk_rgba(156, 193, 220, 255);
			nk_style_from_table(ctx, table);
		}
		else if (theme == eTheme::Dark)
		{
			table[NK_COLOR_TEXT] = nk_rgba(210, 210, 210, 255);
			table[NK_COLOR_WINDOW] = nk_rgba(57, 67, 71, 215);
			table[NK_COLOR_HEADER] = nk_rgba(51, 51, 56, 220);
			table[NK_COLOR_BORDER] = nk_rgba(46, 46, 46, 255);
			table[NK_COLOR_BUTTON] = nk_rgba(48, 83, 111, 255);
			table[NK_COLOR_BUTTON_HOVER] = nk_rgba(58, 93, 121, 255);
			table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(63, 98, 126, 255);
			table[NK_COLOR_TOGGLE] = nk_rgba(50, 58, 61, 255);
			table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 53, 56, 255);
			table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(48, 83, 111, 255);
			table[NK_COLOR_SELECT] = nk_rgba(57, 67, 61, 255);
			table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(48, 83, 111, 255);
			table[NK_COLOR_SLIDER] = nk_rgba(50, 58, 61, 255);
			table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(48, 83, 111, 245);
			table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
			table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
			table[NK_COLOR_PROPERTY] = nk_rgba(50, 58, 61, 255);
			table[NK_COLOR_EDIT] = nk_rgba(50, 58, 61, 225);
			table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
			table[NK_COLOR_COMBO] = nk_rgba(50, 58, 61, 255);
			table[NK_COLOR_CHART] = nk_rgba(50, 58, 61, 255);
			table[NK_COLOR_CHART_COLOR] = nk_rgba(48, 83, 111, 255);
			table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
			table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 58, 61, 255);
			table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(48, 83, 111, 255);
			table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
			table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
			table[NK_COLOR_TAB_HEADER] = nk_rgba(48, 83, 111, 255);
			nk_style_from_table(ctx, table);
		}
		else
		{
			nk_style_default(ctx);
		}
	}
}

/// <summary>
/// Context managers.
/// </summary>
namespace wgui
{
	NuklearGlfwContextManager::~NuklearGlfwContextManager()
	{
        nk_glfw3_shutdown();
	}

	bool NuklearGlfwContextManager::InitWindowContext()
	{
		return mGlfwContext.InitContext();
	}

	bool NuklearGlfwContextManager::InitGlewContext()
	{
		return mGlewContext.InitContext();
	}

	bool NuklearGlfwContextManager::InitNkContext(GLFWwindow* window)
	{
        mNkContext = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
        return mNkContext != nullptr;
	}

    GlfwContextManager::~GlfwContextManager()
    {
        glfwTerminate();
    }

    bool GlfwContextManager::InitContext()
    {
        glfwSetErrorCallback(GlfwErrorCallback);
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, std::get<0>(GlfwVersion));
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, std::get<1>(GlfwVersion));
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		if (OperatingSystem::GetOperatingSystem() == eOsType::MacOS)
		{
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		}

        return glfwInit() == GLFW_TRUE;
    }

    bool GlewContextManager::InitContext()
    {
		if (glewInit() != GLEW_OK) 
		{
			return false;
		}

        glewExperimental = 1;
		return true;
    }
}

/// <summary>
/// Window managers
/// </summary>
namespace wgui
{
	DebugLogger MainWindow::GlfwLogger("GLFW");
	DebugLogger MainWindow::GlLogger("GL");

	MainWindow::MainWindow()
		: WindowBase()
	{
		// Initialize loggers.
        GlfwLogger.setPrefix("\\[[pn]\\]: ");
        GlfwLogger.setLevel(Level::LEVEL_TRACE);
        GlLogger.setPrefix("\\[[pn]\\]: ");
        GlLogger.setLevel(Level::LEVEL_TRACE);
	}

	bool MainWindow::CreateWindow(const std::string& title, 
		int width, int height,
		bool resizable, 
		bool visible, bool decorated, bool fullScreen) 
	{
        GlfwLogger.trace("Setting error callback");
        glfwSetErrorCallback(GlfwErrorCallback);

        GlfwLogger.trace("Initializing");
		if (!mNkContext.InitWindowContext()) 
		{
            GlfwLogger.critical("Failed to initialize graphics window");
			return false;
		}
        
        GlfwLogger.trace("Creating window {int}x{int}", width, height);
        mWindow = glfwCreateWindow(width, height, title.c_str(), fullScreen? glfwGetPrimaryMonitor() : nullptr, nullptr);
        glfwMakeContextCurrent(mWindow);

		if (!mNkContext.InitGlewContext())
		{
			GlLogger.critical("Failed to initialize glew");
			return false;
		}

		GlfwLogger.trace("Creating nk context");
		mNkContext.InitNkContext(mWindow);
		nk_context* ctx = mNkContext.GetContext();
		if (ctx == nullptr)
		{
			GlfwLogger.critical("Failed to initialize nk");
			return false;
		}

        // Glew setup
        glViewport(0, 0, width, height);
        
        // Setup fonts
        GlLogger.trace("Mapping default font");

        struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&atlas);
        struct nk_font *font = nk_font_atlas_add_from_file(atlas, "res/fonts/DroidSans.ttf", 20, 0);
        nk_glfw3_font_stash_end();

        nk_style_load_all_cursors(ctx, atlas->cursors);
        nk_style_set_font(ctx, &font->handle);
        
        SetStyle(ctx, eTheme::Red);

        return true;
	}

	void MainWindow::NewFrame(NuklearWindowRendererBase* layoutRenderer)
	{
		int width, height;
		GetWindowSize(width, height);

		auto* ctx = mNkContext.GetContext();

        // Input
        nk_glfw3_new_frame();
		float delta = mNkContext.GetContext()->delta_time_seconds;
		layoutRenderer->RenderStart(this, ctx, delta);
		layoutRenderer->Render(this, ctx, delta);

        // Draw
        glClear(GL_COLOR_BUFFER_BIT);
        nk_glfw3_render(NK_ANTI_ALIASING_OFF, MaxVertexBuffer, MaxElementBuffer);
		layoutRenderer->RenderFinish(this, ctx, delta);

        glfwSwapBuffers(mWindow);
        glfwPollEvents();
	}

	bool WindowInput::IsKeyDown(nk_keys key) const
	{
		return mWindow->mNkContext.GetContext()->input.keyboard.keys[key].down;
	}

	bool WindowInput::IsKeyClicked(nk_keys key) const
	{
		return mWindow->mNkContext.GetContext()->input.keyboard.keys[key].clicked;
	}

	bool WindowInput::IsMouseButtonDown(nk_buttons button) const
	{
		return mWindow->mNkContext.GetContext()->input.mouse.buttons[button].clicked;
	}

	bool WindowInput::IsMouseButtonClicked(nk_buttons button) const
	{
		return mWindow->mNkContext.GetContext()->input.mouse.buttons[button].clicked;
	}

	void WindowInput::GetKeyboardText(std::string& resultText) const
	{
		nk_context const* ctx = mWindow->mNkContext.GetContext();

		resultText.clear();
		resultText = std::string(ctx->input.keyboard.text, ctx->input.keyboard.text_len);
	}

	void WindowInput::GetMouseDelta(int& deltaX, int& deltaY) const
	{
		deltaX = mWindow->mNkContext.GetContext()->input.mouse.delta.x;
		deltaY = mWindow->mNkContext.GetContext()->input.mouse.delta.y;
	}

	void WindowInput::GetMousePos(int& posX, int& posY) const
	{
		posX = mWindow->mNkContext.GetContext()->input.mouse.pos.x;
		posY = mWindow->mNkContext.GetContext()->input.mouse.pos.y;
	}

	void WindowInput::GetMousePrevPos(int& posX, int& posY) const
	{
		posX = mWindow->mNkContext.GetContext()->input.mouse.prev.x;
		posY = mWindow->mNkContext.GetContext()->input.mouse.prev.y;
	}

	void WindowInput::GetMouseScrollDelta(int& deltaX, int& deltaY) const
	{
		deltaX = mWindow->mNkContext.GetContext()->input.mouse.scroll_delta.x;
		deltaY = mWindow->mNkContext.GetContext()->input.mouse.scroll_delta.y;
	}

	WindowBase::WindowBase()
		: mWindow(nullptr), mWindowTitle(), mNkContext()
	{
	}

	WindowBase::~WindowBase()
	{
		glfwDestroyWindow(mWindow);
	}

	void WindowBase::SetWindowSize(int width, int height)
	{
		glfwSetWindowSize(mWindow, width, height);
	}

	void WindowBase::GetWindowSize(int& width, int& height) const
	{
		glfwGetWindowSize(mWindow, &width, &height);
	}

	void WindowBase::SetWindowPos(int x, int y)
	{
		glfwSetWindowPos(mWindow, x, y);
	}

	void WindowBase::GetWindowPos(int& x, int& y) const
	{
		glfwGetWindowPos(mWindow, &x, &y);
	}

	void WindowBase::SetWindowSizeLimits(int minWidth, int minHeight, int maxWidth, int maxHeight)
	{
		glfwSetWindowSizeLimits(mWindow, minWidth, minHeight, maxWidth, maxHeight);
	}

	void WindowBase::SetWindowTitle(const std::string& title)
	{
		this->mWindowTitle = title;
		glfwSetWindowTitle(mWindow, title.c_str());
	}

	bool WindowBase::GetWindowFocused() const
	{
		return glfwGetWindowAttrib(mWindow, GLFW_FOCUSED);
	}

	bool WindowBase::GetWindowVisible() const
	{
		return glfwGetWindowAttrib(mWindow, GLFW_VISIBLE);
	}

	bool WindowBase::GetWindowResizable() const
	{
		return glfwGetWindowAttrib(mWindow, GLFW_RESIZABLE);
	}

	bool WindowBase::GetWindowDecorated() const
	{
		return glfwGetWindowAttrib(mWindow, GLFW_DECORATED);
	}

	void WindowBase::CloseWindow()
	{
		glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
	}

	void WindowBase::CancelWindowClose()
	{
		glfwSetWindowShouldClose(mWindow, GLFW_FALSE);
	}

	bool WindowBase::Closing() const
	{
		return glfwWindowShouldClose(mWindow);
	}
}

