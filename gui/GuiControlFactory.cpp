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
      { "DynamicGrid", []() { return std::make_unique<wgui::GuiLayoutRowDynamicGrid>(); }},
      { "StaticGrid", []() { return std::make_unique<wgui::GuiLayoutRowStaticGrid>(); }},
      { "VariableGrid", []() { return std::make_unique<wgui::GuiLayoutRowVariableGrid>(); }},
      { "StaticSpace", []() { return std::make_unique<wgui::GuiLayoutStaticSpace>(); }},
      //{ "DynamicSpace", []() { return std::make_unique<wgui::GuiLayoutDynamicSpace>(); }},

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
