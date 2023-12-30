#include "NuklearWindowRenderer.h"
#include "Window.h"
#include "OperatingSystem.h"
#include "App.h"

#define NK_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

std::map<GLFWwindow*, wgui::WindowBase*> wgui::Application::mWindows;

namespace
{
   static constexpr int MaxVertexBuffer = 512 * 1024;
   static constexpr int MaxElementBuffer = 128 * 1024;

   void GlfwErrorCallback(int errCode, const char* msg)
   {
      wgui::Application::Logger.error("{int}: {str}", errCode, msg);
   }

   void DrawFrame(wgui::WindowBase* window, GLFWwindow* gWin, nk_glfw* nkGlfw, wgui::WindowRenderer* layoutRenderer)
   {
      // Render  
      nk_glfw3_new_frame(nkGlfw);
      layoutRenderer->RenderStart(window, &nkGlfw->ctx);
      layoutRenderer->Render(window, &nkGlfw->ctx);

      // Draw
      glClear(GL_COLOR_BUFFER_BIT);
      nk_glfw3_render(nkGlfw, NK_ANTI_ALIASING_ON, MaxVertexBuffer, MaxElementBuffer);
      layoutRenderer->RenderFinish(window, &nkGlfw->ctx);

      glfwSwapBuffers(gWin);
   }

   void WindowResizeCallback(GLFWwindow* window, int width, int height)
   {
      wgui::WindowBase* win = wgui::Application::GetWindow(window);
      if (win->GetRenderer())
      {
         DrawFrame(win, window, win->GetContext().GetGlfw(), win->GetRenderer());
      }
   }

   void WindowMoveCallback(GLFWwindow* window, int width, int height)
   {
      wgui::WindowBase* win = wgui::Application::GetWindow(window);

      win->SetContentScale();
      if (win->GetRenderer())
      {
         DrawFrame(win, window, win->GetContext().GetGlfw(), win->GetRenderer());
      }
   }

   enum class eTheme
   {
      Black,
      White,
      Red,
      Blue,
      Dark,
      Custom
   };

   static void SetStyle(struct nk_context* ctx, enum eTheme theme)
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
      else if (theme == eTheme::Custom)
      {
         // Darker sunset-inspired theme
         struct nk_color table[NK_COLOR_COUNT];
         table[NK_COLOR_TEXT] = nk_rgba(220, 200, 180, 255);               // Text color
         table[NK_COLOR_WINDOW] = nk_rgba(30, 20, 30, 255);                // Window background color
         table[NK_COLOR_HEADER] = nk_rgba(100, 50, 25, 255);               // Header background color
         table[NK_COLOR_BORDER] = nk_rgba(50, 25, 12, 255);                // Border color
         table[NK_COLOR_BUTTON] = nk_rgba(100, 50, 25, 255);               // Button background color
         table[NK_COLOR_BUTTON_HOVER] = nk_rgba(120, 70, 35, 255);         // Button hover background color
         table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(140, 90, 45, 255);        // Button active background color
         table[NK_COLOR_TOGGLE] = nk_rgba(100, 50, 25, 255);               // Toggle background color
         table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(120, 70, 35, 255);         // Toggle hover background color
         table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(140, 90, 45, 255);        // Toggle cursor color
         table[NK_COLOR_SELECT] = nk_rgba(30, 20, 30, 255);               // Select background color
         table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(100, 50, 25, 255);        // Select active background color
         table[NK_COLOR_SLIDER] = nk_rgba(30, 20, 30, 255);               // Slider background color
         table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(100, 50, 25, 255);        // Slider cursor color
         table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(120, 70, 35, 255);// Slider cursor hover color
         table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(140, 90, 45, 255);// Slider cursor active color
         table[NK_COLOR_PROPERTY] = nk_rgba(100, 50, 25, 255);            // Property background color
         table[NK_COLOR_EDIT] = nk_rgba(30, 20, 30, 255);                 // Edit background color
         table[NK_COLOR_EDIT_CURSOR] = nk_rgba(140, 90, 45, 255);        // Edit cursor color
         table[NK_COLOR_COMBO] = nk_rgba(30, 20, 30, 255);                // Combo background color
         table[NK_COLOR_CHART] = nk_rgba(100, 50, 25, 255);               // Chart background color
         table[NK_COLOR_CHART_COLOR] = nk_rgba(140, 90, 45, 255);        // Chart color
         table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 170, 0, 255);// Chart highlight color
         table[NK_COLOR_SCROLLBAR] = nk_rgba(25, 15, 20, 255);            // Scrollbar background color
         table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(100, 50, 25, 255);    // Scrollbar cursor color
         table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(120, 70, 35, 255); // Scrollbar cursor hover color
         table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(140, 90, 45, 255); // Scrollbar cursor active color
         table[NK_COLOR_TAB_HEADER] = nk_rgba(30, 20, 30, 255);           // Tab header background color
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
   void ScaleVec2(struct nk_vec2& result, const struct nk_vec2& base, float scaleX, float scaleY)
   {
      result.x = scaleX * base.x;
      result.y = scaleY * base.y;
   }

   void ScaleButton(nk_style_button& result, const nk_style_button& base, float scaleX, float scaleY)
   {
      result.border = scaleY * base.border;
      result.rounding = scaleY * base.rounding;
      ScaleVec2(result.padding, base.padding, scaleX, scaleY);
      ScaleVec2(result.image_padding, base.image_padding, scaleX, scaleY);
      ScaleVec2(result.touch_padding, base.touch_padding, scaleX, scaleY);
   }

   void ScaleOption(nk_style_toggle& result, const nk_style_toggle& base, float scaleX, float scaleY)
   {

      ScaleVec2(result.padding, base.padding, scaleX, scaleY);
      ScaleVec2(result.touch_padding, base.touch_padding, scaleX, scaleY);
      result.spacing = scaleX * base.spacing; // NOTE: Spacing does not matter for height!
      result.border = scaleY * base.border;
   }

   void ScaleSelectable(nk_style_selectable& result, const nk_style_selectable& base, float scaleX, float scaleY)
   {
      result.rounding = scaleY * base.rounding;

      ScaleVec2(result.padding, base.padding, scaleX, scaleY);
      ScaleVec2(result.touch_padding, base.touch_padding, scaleX, scaleY);
      ScaleVec2(result.image_padding, base.image_padding, scaleX, scaleY);
   }

   void ScaleSlider(nk_style_slider& result, const nk_style_slider& base, float scaleX, float scaleY)
   {
      result.border = scaleY * base.border;
      result.rounding = scaleY * base.rounding;
      result.bar_height = scaleY * base.bar_height;

      ScaleVec2(result.padding, base.padding, scaleX, scaleY);
      ScaleVec2(result.spacing, base.spacing, scaleX, scaleY);
      ScaleVec2(result.cursor_size, base.cursor_size, scaleX, scaleY);
      ScaleButton(result.inc_button, base.inc_button, scaleX, scaleY);
      ScaleButton(result.dec_button, base.dec_button, scaleX, scaleY);
   }

   void ScaleProgress(nk_style_progress& result, const nk_style_progress& base, float scaleX, float scaleY)
   {
      result.border = scaleY * base.border;
      result.rounding = scaleY * base.rounding;
      result.cursor_border = scaleY * base.cursor_border;
      result.cursor_rounding = scaleY * base.cursor_rounding;
      ScaleVec2(result.padding, base.padding, scaleX, scaleY);
   }

   void ScaleScrollbar(nk_style_scrollbar& result, const nk_style_scrollbar& base, float scaleX, float scaleY)
   {
      result.border = scaleY * base.border;
      result.rounding = scaleY * base.rounding;
      result.border_cursor = scaleY * base.border_cursor;
      result.rounding_cursor = scaleY * base.rounding_cursor;
      ScaleVec2(result.padding, base.padding, scaleX, scaleY);
      ScaleButton(result.inc_button, base.inc_button, scaleX, scaleY);
      ScaleButton(result.dec_button, base.dec_button, scaleX, scaleY);
   }

   void ScaleEdit(nk_style_edit& result, const nk_style_edit& base, float scaleX, float scaleY)
   {
      ScaleScrollbar(result.scrollbar, base.scrollbar, scaleX, scaleY);
      result.border = scaleY * base.border;
      result.rounding = scaleY * base.rounding;
      result.cursor_size = scaleY * base.cursor_size;
      ScaleVec2(result.scrollbar_size, base.scrollbar_size, scaleX, scaleY);
      ScaleVec2(result.padding, base.padding, scaleX, scaleY);
      result.row_padding = scaleY * base.row_padding;
   }

   void ScaleProperty(nk_style_property& result, const nk_style_property& base, float scaleX, float scaleY)
   {
      result.border = scaleY * base.border;
      result.rounding = scaleY * base.rounding;
      ScaleVec2(result.padding, base.padding, scaleX, scaleY);
      ScaleEdit(result.edit, base.edit, scaleX, scaleY);
      ScaleButton(result.inc_button, base.inc_button, scaleX, scaleY);
      ScaleButton(result.dec_button, base.dec_button, scaleX, scaleY);
   }
   
   void ScaleChart(nk_style_chart& result, const nk_style_chart& base, float scaleX, float scaleY)
   {
      result.border = scaleY * base.border;
      result.rounding = scaleY * base.rounding;
      ScaleVec2(result.padding, base.padding, scaleX, scaleY);
   }

   void ScaleTab(nk_style_tab& result, const nk_style_tab& base, float scaleX, float scaleY)
   {
      ScaleButton(result.tab_maximize_button, base.tab_maximize_button, scaleX, scaleY);
      ScaleButton(result.tab_minimize_button, base.tab_minimize_button, scaleX, scaleY);
      ScaleButton(result.node_maximize_button, base.node_maximize_button, scaleX, scaleY);
      ScaleButton(result.node_minimize_button, base.node_minimize_button, scaleX, scaleY);
      result.border = scaleY * base.border;
      result.rounding = scaleY * base.rounding;
      result.indent = scaleY * base.indent;
      ScaleVec2(result.padding, base.padding, scaleX, scaleY);
      ScaleVec2(result.spacing, base.spacing, scaleX, scaleY);
   }

   void ScaleCombo(nk_style_combo& result, const nk_style_combo& base, float scaleX, float scaleY)
   {
      ScaleButton(result.button, base.button, scaleX, scaleY);
      result.border = scaleY * base.border;
      result.rounding = scaleY * base.rounding;
      ScaleVec2(result.content_padding, base.content_padding, scaleX, scaleY);
      ScaleVec2(result.button_padding, base.button_padding, scaleX, scaleY);
      ScaleVec2(result.spacing, base.spacing, scaleX, scaleY);
   }

   void ScaleWindow(nk_style_window& result, const nk_style_window& base, float scaleX, float scaleY)
   {
      result.border = scaleY * base.border;
      result.rounding = scaleY * base.rounding;
      result.combo_border = scaleY * base.combo_border;
      result.contextual_border = scaleY * base.contextual_border;
      result.menu_border = scaleY * base.menu_border;
      result.group_border = scaleY * base.group_border;
      result.tooltip_border = scaleY * base.tooltip_border;
      result.popup_border = scaleY * base.popup_border;
      result.min_row_height_padding = scaleY * base.min_row_height_padding;

      ScaleVec2(result.spacing, base.spacing, scaleX, scaleY);
      ScaleVec2(result.scrollbar_size, base.scrollbar_size, scaleX, scaleY);
      ScaleVec2(result.padding, base.padding, scaleX, scaleY);
      ScaleVec2(result.group_padding, base.group_padding, scaleX, scaleY);
      ScaleVec2(result.popup_padding, base.popup_padding, scaleX, scaleY);
      ScaleVec2(result.combo_padding, base.combo_padding, scaleX, scaleY);
      ScaleVec2(result.contextual_padding, base.contextual_padding, scaleX, scaleY);
      ScaleVec2(result.menu_padding, base.menu_padding, scaleX, scaleY);
      ScaleVec2(result.tooltip_padding, base.tooltip_padding, scaleX, scaleY);
   }

   void WindowStyle::Scale(nk_style* style, nk_font* font, float scaleX, float scaleY)
   {
      font->handle.height = mFontSize * scaleY;

      // Text
      style->text.padding = nk_vec2(scaleX * mStyle.text.padding.x, scaleY * mStyle.text.padding.y);

      // Buttons
      ScaleButton(style->button, mStyle.button, scaleX, scaleY);
      ScaleButton(style->contextual_button, mStyle.contextual_button, scaleX, scaleY);
      ScaleButton(style->menu_button, mStyle.menu_button, scaleX, scaleY);

      // Option
      ScaleOption(style->option, mStyle.option, scaleX, scaleY);
      ScaleOption(style->checkbox, mStyle.checkbox, scaleX, scaleY);
      
      ScaleSelectable(style->selectable, mStyle.selectable, scaleX, scaleY);
      ScaleSlider(style->slider, mStyle.slider, scaleX, scaleY);
      ScaleProgress(style->progress, mStyle.progress, scaleX, scaleY);
      ScaleProperty(style->property, mStyle.property, scaleX, scaleY);
      ScaleEdit(style->edit, mStyle.edit, scaleX, scaleY);
      ScaleChart(style->chart, mStyle.chart, scaleX, scaleY);
      ScaleScrollbar(style->scrollh, mStyle.scrollh, scaleX, scaleY);
      ScaleScrollbar(style->scrollv, mStyle.scrollv, scaleX, scaleY);
      ScaleTab(style->tab, mStyle.tab, scaleX, scaleY);
      ScaleCombo(style->combo, mStyle.combo, scaleX, scaleY);

      // Window
      ScaleWindow(style->window, mStyle.window, scaleX, scaleY);
   }

   NuklearGlfwContextManager::NuklearGlfwContextManager()
      : mNkGlfw(std::make_unique<nk_glfw>()),
      mNkContext(nullptr)
   {
      memset(mNkGlfw.get(), 0, sizeof(nk_glfw));
   }

   NuklearGlfwContextManager::~NuklearGlfwContextManager()
   {
      nk_glfw3_shutdown(mNkGlfw.get());
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
      mNkContext = nk_glfw3_init(mNkGlfw.get(), window, NK_GLFW3_INSTALL_CALLBACKS);
      return mNkContext != nullptr;
   }

   GlfwContextManager::~GlfwContextManager()
   {
      glfwTerminate();
   }

   bool GlfwContextManager::InitContext()
   {
      glfwSetErrorCallback(GlfwErrorCallback);

      auto result = glfwInit() == GLFW_TRUE;
      if (result)
      {
         glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, std::get<0>(GlfwVersion));
         glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, std::get<1>(GlfwVersion));
         glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

         if (OperatingSystem::GetOperatingSystem() == eOsType::MacOS)
         {
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
         }

         // Scale to monitor must be set after calling glfwInit()
         glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
         return true;
      }

      return false;
   }

   bool GlewContextManager::InitContext()
   {
      if (glewInit() != GLEW_OK)
      {
         return false;
      }

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
   DebugLogger Application::Logger("Log");

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
      mWindow = glfwCreateWindow(width, height, title.c_str(), fullScreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
      glfwMakeContextCurrent(mWindow);

      if (!mNkContext.InitGlewContext())
      {
         GlLogger.critical("Failed to initialize glew");
         return false;
      }

      GlfwLogger.trace("Creating nk context");
      mNkContext.InitNkContext(mWindow);
      nk_context* ctx = mNkContext.GetContext();
      nk_glfw* nkGlfw = mNkContext.GetGlfw();

      if (ctx == nullptr)
      {
         GlfwLogger.critical("Failed to initialize nk");
         return false;
      }

      // Glew setup
      glViewport(0, 0, width, height);

      // Setup fonts
      GlLogger.trace("Mapping default font");

      // DPI settings
      float scaleX, scaleY;
      glfwGetWindowContentScale(mWindow, &scaleX, &scaleY);

      int fontHeight = 16;
      struct nk_font_config cfg = nk_font_config(fontHeight);
      struct nk_font_atlas* fontAtlas;

      nk_glfw3_font_stash_begin(nkGlfw, &fontAtlas);
      mFont = nk_font_atlas_add_from_file(fontAtlas, 
         "res/fonts/RockoFLF.ttf", fontHeight, &cfg);

      mFont->config->oversample_h = 8;
      mFont->config->oversample_v = 8;
      mFont->config->pixel_snap = true;

      nk_glfw3_font_stash_end(nkGlfw);
      nk_style_set_font(ctx, &mFont->handle);

      SetStyle(ctx, eTheme::Black);

      ctx->style.window.scrollbar_size = nk_vec2(10, 10);
      ctx->style.window.group_padding = nk_vec2(2, 4);
      ctx->style.window.group_border = 4;
      ctx->style.window.spacing = nk_vec2(4, 4);

      // Add the window to the application static data.
      Application::AddWindow(this, mWindow);
      glfwSetWindowSizeCallback(mWindow, WindowResizeCallback);
      glfwSetWindowPosCallback(mWindow, WindowMoveCallback);

      glfwSwapInterval(0);

      // Create window style from the previously set style.
      mWindowStyle = std::make_unique<WindowStyle>(&ctx->style);
      SetContentScale();
      return true;
   }

   void WindowBase::SetContentScale()
   {
      float scaleX, scaleY;
      glfwGetWindowContentScale(mWindow, &scaleX, &scaleY);

      if (scaleX != mContentScaleX || scaleY != mContentScaleY)
      {
         mContentScaleX = scaleX;
         mContentScaleY = scaleY;

         // Scale padding, spacing, font size etc.
         mWindowStyle->Scale(&mNkContext.GetContext()->style, mFont, mContentScaleX, mContentScaleY);
      }
   }

   void MainWindow::Render()
   {
      nk_glfw* nkGlfw = mNkContext.GetGlfw();
      DrawFrame(this, mWindow, nkGlfw, mLastRenderer);
   }

   void WindowBase::Update()
   {
      // TODO: does this need to change for multiple windows?
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

