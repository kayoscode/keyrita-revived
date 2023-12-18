#include "Window.h"
#include "NuklearWindowRenderer.h"

#include "GL/glew.h"
#include "include_nuk.h"

namespace wgui
{
   void NuklearWindowRendererGui::RenderStart(WindowBase const* window, nk_context* ctx, float delta)
   {
   }

   void NuklearWindowRendererGui::Render(WindowBase const* window, nk_context* ctx, float delta)
   {
      int width, height;
      window->GetWindowSize(width, height);

      enum { EASY, HARD };
      static int op = EASY;
      static float value = 0.6f;
      static int i = 20;

      if (nk_begin_titled(ctx, "Show", "title", nk_rect(0, 0, width, height),
         NK_WINDOW_NO_SCROLLBAR)) {
         // fixed widget pixel width
         nk_layout_row_static(ctx, 30, 80, 1);
         if (nk_button_label(ctx, "button")) {
            // event handling
         }

         // fixed widget window ratio width

         nk_layout_row_dynamic(ctx, 200, 2);
         if (nk_group_begin(ctx, "Group", 0))
         {
            float ratios[] = { .25, .75 };
            nk_layout_row(ctx, NK_DYNAMIC, 40, 2, ratios);
            nk_label(ctx, "Volume:", NK_TEXT_LEFT);
            nk_slider_float(ctx, 0, &value, 1.0f, 0.01f);

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

            nk_group_end(ctx);
         }

         // custom widget pixel width
         //nk_layout_row_template_begin(ctx, 40);
         //nk_layout_row_template_push_dynamic(ctx);
         //nk_layout_row_template_push_dynamic(ctx);
         //nk_layout_row_template_end(ctx);
      }
      nk_end(ctx);
   }

   void NuklearWindowRendererGui::RenderFinish(WindowBase const* window, nk_context* context, float delta)
   {
   }
}
