#pragma once

#include <functional>
#include "include_nuk.h"

namespace wgui
{
#pragma region Event data

   class GuiControlBase;

   enum class eEventType
   {
      MouseEnter, MouseLeave, MouseUp, MouseDown, MouseClick, MouseMove,
      KeyUp, KeyDown, KeyClicked,
      HeightChanged, WidthChanged, SizeChanged,
      LayoutUpdated,
      SelectionChanged
   };

   struct EventData
   {
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

#pragma region Event dispatcher

   typedef std::function<void(GuiControlBase* sender, const MouseEventData& eventData)> mouse_enter_event_t;

   template <typename T, eEventType EventType>
   class EventHandler
   {
   public:
      std::vector<T> EventHandlers;
      eEventType EventType = EventType;
   };

   /// <summary>
   /// The event dispatcher should be attached to every control.
   /// Events are processed by each parent until told to stop by the handler function.
   /// If a handler function does not exist, handlers still get propagated to parents.
   /// </summary>
   class EventDispatcher
   {
   public:
      EventDispatcher(GuiControlBase* control)
         : mControl(control)
      {}

      void RegisterMouseEnterEvent(mouse_enter_event_t mouseEvent)
      {
         mMouseEnterEventHandlers.EventHandlers.push_back(mouseEvent);
      }

      void DispatchMouseEnterEvent(nk_context* context);

   private:
      EventHandler<mouse_enter_event_t, eEventType::MouseEnter> mMouseEnterEventHandlers;
      GuiControlBase* mControl;
   };

#pragma endregion
}