#include "Window.h"
#include "NuklearWindowRenderer.h"

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

        if (nk_begin(ctx, "Anything window", nk_rect(0, 0, width, height), 0))
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
    }

    void NuklearWindowRendererGui::RenderFinish(WindowBase const* window, nk_context* context, float delta)
    {
    }
}
