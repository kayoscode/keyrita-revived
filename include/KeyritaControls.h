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
   KeyritaMenu();
   void Render(wgui::WindowBase* const window, nk_context* context);

   std::string GetLabel() override { return "KeyritaMenu"; }
};

