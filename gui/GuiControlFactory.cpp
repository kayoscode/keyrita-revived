#include "NuklearWindowRenderer.h"

namespace
{
   std::map<std::string, 
            std::function<std::unique_ptr<wgui::GuiControlBase>()>, 
            wgui::CaseInsensitiveStrCompare> Factory
   {
      { "Window", []() { return std::make_unique<wgui::GuiLayoutWindow>(); }},

      { "MenuBar", []() { return std::make_unique<wgui::GuiMenuBar>(); }},
      { "Menu", []() { return std::make_unique<wgui::GuiMenu>(); }},
      { "MenuItem", []() { return std::make_unique<wgui::GuiMenuItem>(); }},

      { "DynamicRow", []() { return std::make_unique<wgui::GuiLayoutRowDynamic>(); }},
      { "StaticRow", []() { return std::make_unique<wgui::GuiLayoutRowStatic>(); }},
      { "DynamicGrid", []() { return std::make_unique<wgui::GuiLayoutRowDynamicGrid>(); }},
      { "StaticGrid", []() { return std::make_unique<wgui::GuiLayoutRowStaticGrid>(); }},
      { "VariableGrid", []() { return std::make_unique<wgui::GuiLayoutRowVariableGrid>(); }},
      { "StaticSpace", []() { return std::make_unique<wgui::GuiLayoutStaticSpace>(); }},
      { "DynamicSpace", []() { return std::make_unique<wgui::GuiLayoutDynamicSpace>(); }},

      { "Label", []() { return std::make_unique<wgui::GuiLabel>(); }},
      { "Checkbox", []() { return std::make_unique<wgui::GuiCheckbox>(); }},
      { "HorizontalSeparator", []() { return std::make_unique<wgui::GuiHorizontalSeparator>(); }},
      { "RadioButtonGroup", []() { return std::make_unique<wgui::GuiRadioButtonGroup>(); }},
      { "RadioButton", []() { return std::make_unique<wgui::GuiRadioButton>(); }},
      { "Combobox", []() { return std::make_unique<wgui::GuiCombobox>(); }},
      { "ComboboxItem", []() { return std::make_unique<wgui::GuiComboboxItem>(); }},
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
