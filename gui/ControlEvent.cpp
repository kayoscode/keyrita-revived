#include "ControlEvent.h"
#include "StandardControls.h"

namespace wgui
{
   bool MouseEnterEventObserver::ObserveEvent(WindowBase* window, nk_context* context)
   {
      int mouseX, mouseY;
      window->GetInput().GetMousePos(mouseX, mouseY);

      bool intersecting = IsIntersecting(mWidgetBounds, struct nk_vec2(mouseX, mouseY));

      if (!mPreviouslyInBounds && intersecting)
      {
         return true;
      }

      mPreviouslyInBounds = intersecting;

      return false;
   }

   bool MouseLeaveEventObserver::ObserveEvent(WindowBase* window, nk_context* context)
   {
      int mouseX, mouseY;
      window->GetInput().GetMousePos(mouseX, mouseY);

      bool intersecting = IsIntersecting(mWidgetBounds, struct nk_vec2(mouseX, mouseY));

      if (mPreviouslyInBounds && !intersecting)
      {
         return true;
      }

      mPreviouslyInBounds = intersecting;

      return false;
   }
}