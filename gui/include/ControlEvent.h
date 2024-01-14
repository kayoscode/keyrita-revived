#pragma once

#include "Window.h"
#include "include_nuk.h"
#include <functional>

namespace wgui
{
#pragma region Event data

class GuiControlBase;

struct EventData
{
 public:
   virtual ~EventData() = default;
};

struct MouseEventData : public EventData
{
   int MouseX;
   int MouseY;
};

struct MouseButtonEventData : public MouseEventData
{
   int MouseButton;
   nk_mouse_button MouseButtonData;
};

struct KeyEventData : public EventData
{
   int Key;
   nk_key KeyData;
};

struct LayoutEventData : public EventData
{
};

struct SelectionChangedEventData : public EventData
{
};

#pragma endregion

template <typename T>
   requires std::is_base_of_v<EventData, T>
class EventHandler
{
 public:
   void Register(std::function<void(GuiControlBase *sender, const T &eventData)> callBack)
   {
      mRegistrations.push_back(callBack);
   }

   void SendEvent(GuiControlBase *sender, const T &eventData)
   {
      for (int i = 0; i < mRegistrations; i++)
      {
         mRegistrations[i](sender, eventData);
      }
   }

 private:
   std::vector<std::function<void(GuiControlBase *sender, const T &eventData)>> mRegistrations;
};

#pragma region Event Observers

class EventObserver
{
 public:
   /// <summary>
   /// Check the current state, and see if the event should be triggered right now.
   /// return true if it should be triggered.
   /// </summary>
   /// <param name="context"></param>
   /// <returns></returns>
   virtual bool ObserveEvent(WindowBase *const window, nk_context *context) = 0;

 protected:
   inline bool IsIntersecting(const struct nk_rect &bounds, const struct nk_vec2 mousePos)
   {
      if (mousePos.x >= bounds.x && mousePos.x <= (bounds.x + bounds.w))
      {
         if (mousePos.y >= bounds.y && mousePos.y <= (bounds.y + bounds.h))
         {
            return true;
         }
      }

      return false;
   }
};

class MouseEnterEventObserver : public EventObserver
{
 public:
   virtual bool ObserveEvent(WindowBase *const window, nk_context *context) override;

   void UpdateData(WindowBase *const window, nk_context *context, const struct nk_rect &widgetBounds)
   {
      mWidgetBounds = widgetBounds;
   }

 protected:
   struct nk_rect mWidgetBounds;
   bool mPreviouslyInBounds = false;
};

class MouseLeaveEventObserver : public EventObserver
{
 public:
   virtual bool ObserveEvent(WindowBase *const window, nk_context *context) override;

   void UpdateData(WindowBase *const window, nk_context *context, const struct nk_rect &widgetBounds)
   {
      mWidgetBounds = widgetBounds;
   }

 protected:
   struct nk_rect mWidgetBounds;
   bool mPreviouslyInBounds = false;
};

#pragma endregion

#pragma region Event dispatcher

/// <summary>
/// The event dispatcher should be attached to every control.
/// Events are processed by each parent until told to stop by the handler function.
/// If a handler function does not exist, handlers still get propagated to parents.
/// </summary>
class EventDispatcher
{
 public:
   EventDispatcher() = default;
   ~EventDispatcher() = default;

   template <typename T>
   static void DispatchEvent(GuiControlBase *rootControl, T eventData, const EventHandler<T> &eventHandler)
      requires std::is_base_of_v<EventData, T>
   {
      eventHandler.SendEvent(rootControl, eventData);
   }
};

#pragma endregion
} // namespace wgui