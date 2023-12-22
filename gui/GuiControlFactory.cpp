#include "NuklearWindowRenderer.h"

namespace
{
   std::map<std::string, 
            std::function<std::unique_ptr<wgui::GuiControlBase>()>, 
            wgui::CaseInsensitiveStrCompare> Factory
   {
#pragma region Layouts

      { "Window", []() { return std::make_unique<wgui::GuiLayoutWindow>(); }},
      { "DynamicRow", []() { return std::make_unique<wgui::GuiLayoutRowDynamic>(); }},
      { "StaticRow", []() { return std::make_unique<wgui::GuiLayoutRowStatic>(); }},

#pragma endregion

#pragma region Controls

      { "label", []() { return std::make_unique<wgui::GuiLabel>(); }},

#pragma endregion
   };
}

namespace wgui
{
   std::unique_ptr<GuiControlBase> XmlLoadedWindowRenderer::StandardControlFactory::CreateControl(const std::string& controlName)
   {
      const auto& foundControl = Factory.find(controlName);

      if (foundControl != Factory.end())
      {
         return std::move(foundControl->second());
      }

      return nullptr;
   }
}
