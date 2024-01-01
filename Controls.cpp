#include "KeyritaControls.h"
#include "XmlToUi.h"

#include "ControlAccessUtils.h"

using namespace wgui;

void KeyritaControlsFactory::Init()
{
   RegisterControl<KeyritaMenu>();
   RegisterControl<ExampleUi>();
}

void KeyritaMenu::Init()
{
   XmlToUiUtil::ConstructLayoutFromXmlFile("./res/gui/KeyritaMenu.guix", mOwnedControls, mControls);
}

void KeyritaMenu::ChildRender(WindowBase* const window, nk_context* context)
{
   for (int i = 0; i < mControls.size(); i++)
   {
      mControls[i]->Render(window, context);
   }
}

void ExampleUi::Init()
{
   XmlToUiUtil::ConstructLayoutFromXmlFile("./res/gui/ExampleUI.guix", mOwnedControls, mControls);
}

void ExampleUi::ChildRender(WindowBase* const window, nk_context* context)
{
   for (int i = 0; i < mControls.size(); i++)
   {
      mControls[i]->Render(window, context);
   }
}

