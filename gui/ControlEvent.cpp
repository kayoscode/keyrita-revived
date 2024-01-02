#include "ControlEvent.h"
#include "StandardControls.h"

namespace wgui
{
   void EventDispatcher::DispatchMouseEnterEvent(nk_context* context)
   {
      MouseEventData data;
      data.MouseX = context->input.mouse.pos.x;
      data.MouseY = context->input.mouse.pos.y;

      for (int i = 0; i < mMouseEnterEventHandlers.EventHandlers.size(); i++)
      {
         mMouseEnterEventHandlers.EventHandlers[i](mControl, data);
      }
   }
}