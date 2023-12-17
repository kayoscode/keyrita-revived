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

	void ErrorCallback(int errCode, const char* msg)
	{
		GlfwLogger.critical("{int}: {str}", errCode, msg);
	}
}

int main()
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
	
	struct nk_glfw glfw = { 0 };
	static GLFWwindow* win;
	GlfwLogger.trace("Setting error callback");
	glfwSetErrorCallback(ErrorCallback);
	
	GlfwLogger.trace("Initializing");
	if (!glfwInit())
	{
		GlfwLogger.critical("Failed to initialize glfw");
		exit(1);
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
		exit(1);
	}
	
	GlLogger.trace("Creating gl context");
	struct nk_context *ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
	
	// Setup fonts
	GlLogger.trace("Mapping fonts");
    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
    /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 14, 0);*/
    /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
    /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
    /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
    /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
    nk_glfw3_font_stash_end();
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    /*nk_style_set_font(ctx, &droid->handle);*/

    while (!glfwWindowShouldClose(win))
    {
		// Input
        glfwPollEvents();
        nk_glfw3_new_frame();
		
		// Gui
		if (nk_begin(ctx, "Keyrita", nk_rect(0, 0, windowWidth, windowHeight), 0))
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
