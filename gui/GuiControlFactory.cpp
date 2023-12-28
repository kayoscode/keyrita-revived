#include "NuklearWindowRenderer.h"

namespace wgui
{
   void XmlLoadedWindowRenderer::StandardControlFactory::Init()
   {
      RegisterControl<GuiLayoutWindow>();

      RegisterControl<GuiMenuBar>();
      RegisterControl<GuiMenu>();
      RegisterControl<GuiMenuItem>();

      RegisterControl<GuiLayoutRowDynamic>();
      RegisterControl<GuiLayoutRowStatic>();
      RegisterControl<GuiLayoutRowDynamicGrid>();
      RegisterControl<GuiLayoutRowStaticGrid>();
      RegisterControl<GuiLayoutRowVariableGrid>();
      RegisterControl<GuiLayoutStaticSpace>();
      RegisterControl<GuiLayoutDynamicSpace>();
      RegisterControl<GuiLayoutGroup>();
      RegisterControl<GuiLayoutTreeNode>();
      RegisterControl<GuiLayoutTreeTab>();

      RegisterControl<GuiLabel>();
      RegisterControl<GuiSelectableLabel>();
      RegisterControl<GuiButton>();
      RegisterControl<GuiCheckbox>();
      RegisterControl<GuiHorizontalSeparator>();
      RegisterControl<GuiRadioButtonGroup>();
      RegisterControl<GuiRadioButton>();
      RegisterControl<GuiCombobox>();
      RegisterControl<GuiComboboxItem>();
      RegisterControl<GuiSliderInt>();
      RegisterControl<GuiSliderReal>();
      RegisterControl<GuiProgressBar>();
      RegisterControl<GuiInputInt>();
      RegisterControl<GuiInputReal>();
   }
}
