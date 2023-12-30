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

#include "StandardControls.h"

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
      virtual void Init() = 0;
      virtual void RenderStart(WindowBase* const window, nk_context* context) = 0;
      virtual void Render(WindowBase* const window, nk_context* context) = 0;
      virtual void RenderFinish(WindowBase* const window, nk_context* context) = 0;

   private:
   };

   class GuiControlFactoryBase
   {
   public:
      std::unique_ptr<GuiControlBase> CreateControl(const std::string& controlType)
      {
         if (mFactory.find(controlType) != mFactory.end())
         {
            return mFactory[controlType]();
         }

         return nullptr;
      }

      std::vector<std::string> ListRegisteredControls()
      {
         std::vector<std::string> registeredControls;

         for (auto& ctrl : mFactory)
         {
            registeredControls.push_back(ctrl.first);
         }

         return registeredControls;
      }

      /// <summary>
      /// Register all the controls this factory can build to the system.
      /// </summary>
      virtual void Init() = 0;

   protected:
      template<typename T>
      void RegisterControl() 
         requires std::is_base_of_v<GuiControlBase, T>
      {
         std::unique_ptr<T> ctrl = std::make_unique<T>();
         mFactory.emplace(ctrl->GetLabel(), 
            []() -> std::unique_ptr<GuiControlBase>
            {
               std::unique_ptr<GuiControlBase> ctrl = std::make_unique<T>();
               ctrl->Init();
               return ctrl;
            });
      }

      std::map<std::string, std::function<std::unique_ptr
         <wgui::GuiControlBase>()>, CaseInsensitiveStrCompare> mFactory;
   };

   /// <summary>
   /// Renders the gui based on the controls and layouts given by the developer.
   /// </summary>
   class StandardGuiRenderer : public WindowRenderer
   {
   public:
      StandardGuiRenderer()
      {
      }

      void Init() override;
      void RenderStart(WindowBase* const window, nk_context* context) override;
      void Render(WindowBase* const window, nk_context* context) override;
      void RenderFinish(WindowBase* const window, nk_context* delta) override;

      bool AddChild(GuiControlBase* window)
      {
         mControls.push_back(window);
         return true;
      }

   protected:
      std::vector<GuiControlBase*> mControls;
   };

   /// <summary>
   /// A window renderer that loads its controls from an XML file.
   /// </summary>
   class XmlRenderer : public StandardGuiRenderer
   {
   public:
      bool ConstructLayoutFromXmlFile(const std::string& fileName);

      void AddControl(std::unique_ptr<GuiControlBase> window)
      {
         GuiControlBase* pWin = window.get();
         mOwnedControls.push_back(std::move(window));
         StandardGuiRenderer::AddChild(pWin);
      }

   private:
      std::vector<std::unique_ptr<GuiControlBase>> mOwnedControls;
   };
}
