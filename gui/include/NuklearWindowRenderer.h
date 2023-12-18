#pragma once

struct nk_context;

namespace wgui
{
    class WindowBase;

    /// <summary>
    /// Base class for all nuklear window render handlers.
    /// The implementation of this class can be as simple as just drawing a textbox or even doing nothing
    /// and it can be as complex as holding a structured list of controls and rendering them and updating them.
    /// It can be more complex than that!
    /// 
    /// This class is designed to be a layer above the game loop and allow developers to abstract
    /// their GUI rendering logic from the rest of their application's code.
    /// There is no implementation for this class, just a header file for clients to implement in their own way.
    /// 
    /// This class needs:
    /// 1. A callback to the main render process
    /// 2. A callback to the initialization process
    /// 3. A callback to the render cleanup process (moving to the next frame/etc/)
    /// 4. A pointer to a const GlfwWindow
    /// 5. Overridable callbacks to all windows events.
    /// 6. The rest is up to the implementer!
    /// </summary>
    class NuklearWindowRendererBase
    {
    public:
        virtual void RenderStart(WindowBase const* window, nk_context* ctx, float delta) = 0;
        virtual void Render(WindowBase const* window, nk_context* ctx, float delta) = 0;
        virtual void RenderFinish(WindowBase const* window, nk_context* ctx, float delta) = 0;

    private:
    };

    /// <summary>
    /// Renders the gui based on the controls and layouts given by the developer.
    /// </summary>
    class NuklearWindowRendererGui : public NuklearWindowRendererBase
    {
    public:
        void RenderStart(WindowBase const* window, nk_context* ctx, float delta) override;
        void Render(WindowBase const* window, nk_context* ctx, float delta) override;
        void RenderFinish(WindowBase const* window, nk_context* ctx, float delta) override;

    private:
    };
}
