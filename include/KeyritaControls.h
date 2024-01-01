#pragma once

#include "NuklearWindowRenderer.h"

class KeyritaControlsFactory : public wgui::GuiControlFactoryBase
{
public:
   void Init() override;
};

class KeyritaMenu : public wgui::GuiAbstractControl
{
public:
   void ChildRender(wgui::WindowBase* const window, nk_context* context) override;

   void Init() override;
   std::string GetLabel() const override { return "KeyritaMenu"; }
};

class ExampleUi : public wgui::GuiAbstractControl
{
public:
   void ChildRender(wgui::WindowBase* const window, nk_context* context) override;

   void Init() override;
   std::string GetLabel() const override { return "ExampleUI"; }
};

