#include "KeyritaControls.h"
#include "XmlToUi.h"

using namespace wgui;

void KeyritaControlsFactory::Init()
{
   RegisterControl<KeyritaMenu>();
}

KeyritaMenu::KeyritaMenu()
{
   XmlToUiUtil::ConstructLayoutFromXmlFile("./res/gui/KeyritaMenu.guix", mOwnedControls, mControls);
}

void KeyritaMenu::Render(WindowBase* const window, nk_context* context)
{
   for (int i = 0; i < mControls.size(); i++)
   {
      mControls[i]->Render(window, context);
   }
}

