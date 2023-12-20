#include "Window.h"
#include "NuklearWindowRenderer.h"

#include "GL/glew.h"
#include "include_nuk.h"

namespace wgui
{
   void WindowRendererGui::RenderStart(WindowBase* const window, nk_context* context)
   {
   }

   void WindowRendererGui::Render(WindowBase* const window, nk_context* context)
   {
      for (const auto& nextWindow : mWindows)
      {
         nextWindow->Render(window, context);
      }
   }

   void WindowRendererGui::RenderFinish(WindowBase* const window, nk_context* context)
   {
   }

   void GuiWindow::Render(WindowBase* window, nk_context* context)
   {
      // 0 flags for now! We will have to fix that.
      if (nk_begin_titled(context, mName.c_str(), mTitle.c_str(), nk_rect(mPosX, mPosY, mWidth, mHeight), 0))
      {
         for (const auto& control : mControls)
         {
            control->Render(window, context);
         }
      }
      nk_end(context);
   }

#pragma region Gui Controls

   void GuiLabel::Render(WindowBase* const window, nk_context* context)
   {
      nk_label(context, mText.c_str(), static_cast<nk_flags>(mTextAlignment));
   }

#pragma endregion

#pragma region Layout Row Implementations

   void GuiLayoutRowDynamic::Render(WindowBase* const window, nk_context* context)
   {
      nk_layout_row_dynamic(context, mHeight, std::max(static_cast<int>(mControls.size()), mMinCols));
      RenderControls(window, context);
      //nk_layout_row_end(context);
   }

   void GuiLayoutRowStatic::Render(WindowBase* const window, nk_context* context)
   {
      nk_layout_row_static(context, mHeight, mMinCols, std::max(static_cast<int>(mControls.size()), mMinCols));
      RenderControls(window, context);
      //nk_layout_row_end(context);
   }

#pragma endregion
}
