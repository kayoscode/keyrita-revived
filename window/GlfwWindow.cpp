#include "GL/glew.h"
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT

#include <iostream>
#include "nuklear_glfw_gl3.h"
#include "DebugLogger.h"

namespace 
{
	DebugLogger GlfwLogger("GLFW", Level::LEVEL_TRACE);
	DebugLogger GlLogger("GL");
	DebugLogger GeneralLogger("LOG");

	void ErrorCallback(int errCode, const char *msg)
	{
		GlfwLogger.critical("{int}: {str}", errCode, msg);
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

extern int CreateWindow()
{
	GlfwLogger.setPrefix("\\[[pn]\\]: ");
	GlfwLogger.setLevel(Level::LEVEL_TRACE);
	GlLogger.setPrefix("\\[[pn]\\]: ");
	GlLogger.setLevel(Level::LEVEL_TRACE);
	GeneralLogger.setPrefix("\\[[pn]\\]: ");
	GeneralLogger.setLevel(Level::LEVEL_TRACE);

	GeneralLogger.trace("App startup");

	const int windowWidth = 800;
	const int windowHeight = 600;
	
	static GLFWwindow* win;
	GlfwLogger.trace("Setting error callback");
	glfwSetErrorCallback(ErrorCallback);
	
	GlfwLogger.trace("Initializing");
	if (!glfwInit())
	{
		GlfwLogger.critical("Failed to initialize glfw");
		return 1;
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	GlfwLogger.trace("Apple detected -> configuring for apple");
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	
	GlfwLogger.trace("Creating window {int}x{int}", windowWidth, windowHeight);
	win = glfwCreateWindow(windowWidth, windowHeight, "Keyrita", nullptr, nullptr);
	glfwMakeContextCurrent(win);

	// Glew setup
    glViewport(0, 0, windowWidth, windowHeight);
	glewExperimental = 1;
	GlLogger.trace("Creating glew in experimental mode");
	GlLogger.trace("Initializing");
	if (glewInit() != GLEW_OK)
	{
		GlLogger.critical("Failed to initialize glew");
		return 1;
	}
	
	GlLogger.trace("Creating gl context");
	struct nk_context *ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
	
	// Setup fonts
	GlLogger.trace("Mapping fonts");

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    struct nk_font *font = nk_font_atlas_add_from_file(atlas, "res/fonts/Comfortaa_Regular.ttf", 22, 0);
    nk_glfw3_font_stash_end();

    nk_style_load_all_cursors(ctx, atlas->cursors);
    nk_style_set_font(ctx, &font->handle);
	
	SetStyle(ctx, eTheme::Dark);

    while (!glfwWindowShouldClose(win))
    {
		// Input
        glfwPollEvents();
        nk_glfw3_new_frame();

		if (nk_begin(ctx, "Anything window", nk_rect(0, 0, windowWidth, windowHeight), 0))
		{
			nk_layout_row_dynamic(ctx, 120, 1);
			nk_label(ctx, "Hello, World", NK_TEXT_CENTERED);

			nk_layout_row_dynamic(ctx, 30, 2);
			nk_label(ctx, "Hello, World", NK_TEXT_CENTERED);
			nk_label(ctx, "Hello, World", NK_TEXT_CENTERED);

			nk_layout_row_dynamic(ctx, 30, 1);
			if (nk_button_label(ctx, "button"))
			{
				std::cout << "Pressed button\n";
			}
		}
		nk_end(ctx);
		
		// Draw
		glClear(GL_COLOR_BUFFER_BIT);
		nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
		glfwSwapBuffers(win);
	}
	
	GlfwLogger.trace("Stopping application");
	nk_glfw3_shutdown();
	glfwTerminate();

	return 0;
}
