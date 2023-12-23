#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <exception>
#include <map>
#include <functional>
#include <algorithm>
#include <stdint.h>
#include <memory>

#include "pugixml.hpp"
#include "Attributes.h"
#include "StandardControls.h"

#include "Window.h"

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
   class WindowRenderer
   {
   public:
      virtual void RenderStart(WindowBase* const window, nk_context* context) = 0;
      virtual void Render(WindowBase* const window, nk_context* context) = 0;
      virtual void RenderFinish(WindowBase* const window, nk_context* context) = 0;

   private:
   };

   class GuiControlFactory
   {
   public:
      virtual std::unique_ptr<GuiControlBase> CreateControl(const std::string& controlType) = 0;
   };

   /// <summary>
   /// Renders the gui based on the controls and layouts given by the developer.
   /// </summary>
   class WindowRendererGui : public WindowRenderer
   {
   public:
      WindowRendererGui()
      {
      }

      void RenderStart(WindowBase* const window, nk_context* context) override;
      void Render(WindowBase* const window, nk_context* context) override;
      void RenderFinish(WindowBase* const window, nk_context* delta) override;

      bool AddChild(GuiControlBase* window)
      {
         mWindows.push_back(window);
         return true;
      }

   private:
      std::vector<GuiControlBase*> mWindows;
   };

   /// <summary>
   /// A window renderer that loads its controls from an XML file.
   /// </summary>
   class XmlLoadedWindowRenderer : public WindowRendererGui
   {
   private:
      class StandardControlFactory : public GuiControlFactory
      {
      public:
         std::unique_ptr<GuiControlBase> CreateControl(const std::string& controlType);
      };

   public:
      XmlLoadedWindowRenderer()
      {
         AddControlFactory<StandardControlFactory>();
      }

      bool ConstructLayoutFromXml(const std::string& xml);
      bool ConstructLayoutFromXmlFile(const std::string& fileName);

      template <class T>
      void AddControlFactory() requires std::is_convertible_v<T, StandardControlFactory>
      {
         mControlFactories.push_back(std::make_unique<T>());
      }

      void AddControl(std::unique_ptr<GuiControlBase> window)
      {
         GuiControlBase* pWin = window.get();
         mOwnedControls.push_back(std::move(window));
         WindowRendererGui::AddChild(pWin);
      }

   private:
      std::vector<std::unique_ptr<GuiControlBase>> mOwnedControls;
      std::vector<std::unique_ptr<GuiControlFactory>> mControlFactories;

      void ConstructWindows(const pugi::xml_object_range<pugi::xml_node_iterator>& document);
      void ConstructComponents(const pugi::xml_object_range<pugi::xml_node_iterator>& document,
         GuiControlBase* parent);

      std::unique_ptr<GuiControlBase> CreateControl(const std::string& controlName)
      {
         for (const auto& factory : mControlFactories)
         {
            std::unique_ptr<GuiControlBase> newControl = factory->CreateControl(controlName);

            if (newControl != nullptr)
            {
               return std::move(newControl);
            }
         }

         return nullptr;
      }
   };
}
