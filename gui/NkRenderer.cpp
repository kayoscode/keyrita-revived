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

   void GuiLayoutWindow::Render(WindowBase* window, nk_context* context)
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
      nk_layout_row_dynamic(context, mHeight, mControls.size());
      RenderControls(window, context);
   }

   void GuiLayoutRowStatic::Render(WindowBase* const window, nk_context* context)
   {
      nk_layout_row_static(context, mHeight, mColWidth, mControls.size());
      RenderControls(window, context);
   }

   void GuiLayoutRowDynamicGrid::Render(WindowBase* const window, nk_context* context)
   {
      if (mControls.size() > mScales.size())
      {
         throw std::runtime_error("More controls exist than scales.");
      }

      // Min cols is ignored here.
      nk_layout_row_begin(context, NK_DYNAMIC, mHeight, mControls.size());

      // There's guaranteed to be one scale for every control.
      // There might be more scales set, but we can safely ignore those.
      for (int i = 0; i < mControls.size(); i++)
      {
         nk_layout_row_push(context, (float)*mScales[i]);
         mControls[i]->Render(window, context);
      }

      nk_layout_row_end(context);
   }

   void GuiLayoutRowStaticGrid::Render(WindowBase* const window, nk_context* context)
   {
      if (mControls.size() > mScales.size())
      {
         throw std::runtime_error("More controls exist than scales.");
      }

      // Min cols is ignored here.
      nk_layout_row_begin(context, NK_STATIC, mHeight, mControls.size());

      // There's guaranteed to be one scale for every control.
      // There might be more scales set, but we can safely ignore those.
      for (int i = 0; i < mControls.size(); i++)
      {
         nk_layout_row_push(context, (int)*mScales[i]);
         mControls[i]->Render(window, context);
      }

      nk_layout_row_end(context);
   }

   void GuiLayoutRowVariableGrid::Render(WindowBase* const window, nk_context* context)
   {
      if (mControls.size() > mScales.size())
      {
         throw std::runtime_error("More controls exist than scales.");
      }

      // Min cols is ignored here.
      nk_layout_row_template_begin(context, mHeight);

      for (int i = 0; i < mControls.size(); i++)
      {
         nk_layout_row_template_push_variable(context, *mScales[i]);
      }

      nk_layout_row_template_end(context);

      // There's guaranteed to be one scale for every control.
      // There might be more scales set, but we can safely ignore those.
      for (int i = 0; i < mControls.size(); i++)
      {
         mControls[i]->Render(window, context);
      }
   }

   void GuiLayoutStaticSpace::Render(WindowBase* const window, nk_context* context)
   {
      if (mControls.size() > std::min(mWidths.size(), mHeights.size()))
      {
         throw std::runtime_error("More controls exist than scales.");
      }
   }

#pragma endregion
}
