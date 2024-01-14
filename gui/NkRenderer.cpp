#include <cassert>
#include <functional>

#include "ControlAccessUtils.h"
#include "NuklearWindowRenderer.h"
#include "Window.h"
#include "XmlToUi.h"

#include "GL/glew.h"
#include "include_nuk.h"

namespace
{
using namespace wgui;

nk_flags WinFlagsToNkWinFlags(int winFlags)
{
   nk_flags result = 0;

   if (winFlags & static_cast<int>(eWindowFlags::Header))
   {
      result |= NK_WINDOW_TITLE;
   }

   if (winFlags & static_cast<int>(eWindowFlags::ScrollbarAutoHide))
   {
      result |= NK_WINDOW_SCROLL_AUTO_HIDE;
   }

   if (winFlags & static_cast<int>(eWindowFlags::InBackground))
   {
      result |= NK_WINDOW_BACKGROUND;
   }

   return result;
}
} // namespace

namespace wgui
{
void StandardControlFactory::StandardControlFactory::Init()
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
   RegisterControl<GuiSpace>();
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

void ControlTreeIterator::BuildPath()
{
   mPath.push({{}, 0, 0});

   if (!mCurrent)
   {
      return;
   }

   mCurrent->ForEachChild([&](GuiControlBase *child, int childIndex) {
      mPath.top().Children.push_back(child);
      mPath.top().ChildrenCount++;
   });
}

void StandardGuiRenderer::Init()
{
   for (int i = 0; i < mControls.size(); i++)
   {
      for (auto it = mControls[i]->begin(); it != mControls[i]->end(); ++it)
      {
         (*it)->OnInitialized();
      }
   }
}

void StandardGuiRenderer::RenderStart(WindowBase *const window, nk_context *context)
{
}

void StandardGuiRenderer::Render(WindowBase *const window, nk_context *context)
{
   for (const auto &nextWindow : mControls)
   {
      nextWindow->Render(window, context);
   }
}

void StandardGuiRenderer::RenderFinish(WindowBase *const window, nk_context *context)
{
}

float ChildSupportingGuiControlBase::GetMaxChildHeight(WindowBase *const window, nk_context *context) const
{
   float maxHeight = 0;

   for (int i = 0; i < mControls.size(); i++)
   {
      float ht = mControls[i]->GetHeight(window, context);
      ht += mControls[i]->GetVerticalSpacing(window, context);
      if (ht > maxHeight)
      {
         maxHeight = ht;
      }
   }

   return maxHeight;
}

float ChildSupportingGuiControlBase::GetTotalChildHeight(WindowBase *const window, nk_context *context) const
{
   float totalHeight = 0;

   for (int i = 0; i < mControls.size(); i++)
   {
      totalHeight += mControls[i]->GetHeight(window, context);
      totalHeight += mControls[i]->GetVerticalSpacing(window, context);
   }

   return totalHeight;
}

void ChildSupportingGuiControlBase::ForEachChild(std::function<void(GuiControlBase *child, int index)> function)
{
   for (int i = 0; i < mControls.size(); i++)
   {
      function(mControls[i], i);
   }
}

void GuiControlBase::HandleEvents(WindowBase *window, nk_context *context)
{
   // Handle common event listeners.
   // Mouse entered event.
   ChildHandleEvents(window, context);
}

void GuiControlBase::Render(WindowBase *const window, nk_context *context)
{
   if (!mEnabled)
   {
      nk_widget_disable_begin(context);
      ChildRender(window, context);
      nk_widget_disable_end(context);
   }
   else
   {
      ChildRender(window, context);
      HandleEvents(window, context);
   }
}

void GuiLayoutWindow::ChildRender(WindowBase *window, nk_context *context)
{
   int width, height, posX, posY;

   if (mTrackParentWindowSize)
   {
      int w, h;
      window->GetWindowSize(w, h);
      mWidth = w;
      mHeight = h;
      mPosX = 0;
      mPosY = 0;

      width = mWidth;
      height = mHeight;
      posX = mPosX;
      posY = mPosY;
   }
   else
   {
      height = GetHeight(window, context);
      width = mWidth * window->GetContentScaleX();
      posX = mPosX * window->GetContentScaleX();
      posY = mPosY * window->GetContentScaleY();
   }

   nk_flags flags = WinFlagsToNkWinFlags(mFlags);

   flags |= (!mScrollable) ? NK_WINDOW_NO_SCROLLBAR : 0;
   flags |= mBorder ? NK_WINDOW_BORDER : 0;

   if (!window->GetWindowFocused())
   {
      flags |= NK_WINDOW_NOT_INTERACTIVE;
      flags |= NK_WINDOW_NO_INPUT;
   }
   else
   {
      nk_window *win = nk_window_find(context, mName.c_str());
      if (win)
      {
         win->flags &= ~NK_WINDOW_NOT_INTERACTIVE;
         win->flags &= ~NK_WINDOW_NO_INPUT;
      }
   }

   // 0 flags for now! We will have to fix that.
   if (nk_begin_titled(context, mWindowName.c_str(), mTitle.c_str(), nk_rect(posX, posY, width, height), flags))
   {
      for (const auto &control : mControls)
      {
         control->Render(window, context);
      }
   }
   nk_end(context);
}

#pragma region Gui Controls

void GuiLabel::ChildRender(WindowBase *const window, nk_context *context)
{
   nk_label(context, mText.c_str(), static_cast<nk_flags>(mTextAlignFlags));
}

void GuiSpace::ChildRender(WindowBase *const window, nk_context *context)
{
   nk_spacing(context, 1);
}

void GuiHorizontalSeparator::ChildRender(WindowBase *const window, nk_context *context)
{
   struct nk_rect bounds;
   auto state = nk_widget_fitting(&bounds, context, nk_vec2(0, 0));

   // Draw horizontal separator.
   struct nk_window *win = context->current;
   const struct nk_style *style = &context->style;

   float thickness = GetHeight(window, context);

   nk_stroke_line(&win->buffer, bounds.x, bounds.y + (thickness / 2.0), bounds.x + bounds.w,
                  bounds.y + (thickness / 2.0), thickness, nk_color(0, 0, 0, 255 / 3));
}

void GuiButton::ChildRender(WindowBase *const window, nk_context *context)
{
   struct nk_rect bounds = nk_widget_bounds(context);

   nk_button_label(context, mText.c_str());
}

void GuiCheckbox::ChildRender(WindowBase *const window, nk_context *context)
{
   nk_bool checked = mChecked;
   nk_checkbox_label(context, mText.c_str(), &checked);
   mChecked = checked;
}

void GuiRadioButton::ChildRender(WindowBase *const window, nk_context *context)
{
   assert(mRadioButtonSelection != nullptr);

   if (nk_option_label(context, mText.c_str(), mButtonIndex == *mRadioButtonSelection))
   {
      *mRadioButtonSelection = mButtonIndex;
   }
}

void GuiRadioButtonGroup::OnInitialized()
{
   mRadioButtons.clear();

   auto *radioBtns = &mRadioButtons;
   int64_t *currentSelection = &mCurrentSelection;
   int buttonIndex = 1;

   std::function<void(GuiControlBase *, int index)> func = [&func, radioBtns, currentSelection,
                                                            &buttonIndex](GuiControlBase *control, int index) -> void {
      GuiRadioButton *rb = dynamic_cast<GuiRadioButton *>(control);
      if (rb != nullptr)
      {
         rb->SetRadioButtonGroupSelection(currentSelection, buttonIndex);
         buttonIndex++;
         radioBtns->push_back(rb);
      }

      if (control != nullptr && control->SupportsChildren())
      {
         control->ForEachChild(func);
      }
   };

   func(this, 0);
}

void GuiRadioButtonGroup::ChildRender(WindowBase *const window, nk_context *context)
{
   for (int i = 0; i < mControls.size(); i++)
   {
      mControls[i]->Render(window, context);
   }
}

void GuiCombobox::OnInitialized()
{
   mComboboxItems.clear();
   mComboboxTexts.clear();

   auto *items = &mComboboxItems;
   auto *texts = &mComboboxTexts;
   int64_t *currentSelection = &mSelectedItem;
   int64_t *itemHeight = &mItemHeight;
   int itemIndex = 1;

   std::function<void(GuiControlBase *, int index)> func = [&func, items, texts, currentSelection, itemHeight,
                                                            &itemIndex](GuiControlBase *control, int index) -> void {
      GuiComboboxItem *cb = dynamic_cast<GuiComboboxItem *>(control);
      if (cb != nullptr)
      {
         cb->SetComboboxSelectedItem(currentSelection, itemHeight, itemIndex);
         itemIndex++;
         items->push_back(cb);

         texts->push_back(&cb->GetOrCreateAttribute<std::string, AttrString>((std::string)GuiLabel::TextAttr));
      }

      if (control != nullptr && control->SupportsChildren())
      {
         control->ForEachChild(func);
      }
   };

   func(this, 0);
}

void GuiComboboxItem::ChildRender(WindowBase *const window, nk_context *context)
{
   int itemHeight = GetHeight(window, context);
   nk_layout_row_dynamic(context, itemHeight, 1);

   if (nk_combo_item_label(context, mText.c_str(), mTextAlignment))
   {
      *mComboboxSelectedItem = mItemIndex;
   }
}

void GuiCombobox::ChildRender(WindowBase *const window, nk_context *context)
{
   std::string text = "";
   assert(mComboboxItems.size() == mComboboxTexts.size());
   assert(mSelectedItem <= mComboboxItems.size());

   if (mComboboxTexts.size() > 0)
   {
      text = *mComboboxTexts[mSelectedItem - 1];
   }

   int width = mWidth * window->GetContentScaleX();
   int height = mHeight * window->GetContentScaleY();
   if (nk_combo_begin_label(context, text.c_str(), nk_vec2(width, height)))
   {
      for (int i = 0; i < mControls.size(); i++)
      {
         mControls[i]->Render(window, context);
      }

      nk_combo_end(context);
   }
}

void GuiSliderInt::ChildRender(WindowBase *const window, nk_context *context)
{
   int value = mValue;
   nk_slider_int(context, mMinValue, &value, mMaxValue, mStep);
   mValue = value;
}

void GuiSliderReal::ChildRender(WindowBase *const window, nk_context *context)
{
   float value = mValue;
   nk_slider_float(context, mMinValue, &value, mMaxValue, mStep);
   mValue = value;
}

void GuiProgressBar::ChildRender(WindowBase *const window, nk_context *context)
{
   nk_size current = static_cast<nk_size>(mValue);
   nk_size max = static_cast<nk_size>(mMaxValue);
   nk_progress(context, &current, max, mModifiable);
   mValue = static_cast<int64_t>(current);
}

void GuiInputInt::ChildRender(WindowBase *const window, nk_context *context)
{
   double stepPerPixel = mStepPerPx / window->GetContentScaleX();

   mValue = nk_propertyi(context, ("#" + mName).c_str(), mMinValue, mValue, mMaxValue, mStep, stepPerPixel);
}

void GuiInputReal::ChildRender(WindowBase *const window, nk_context *context)
{
   double stepPerPixel = mStepPerPx / window->GetContentScaleX();

   mValue = nk_propertyd(context, ("#" + mName).c_str(), mMinValue, mValue, mMaxValue, mStep, stepPerPixel);
}

void GuiSelectableLabel::ChildRender(WindowBase *const window, nk_context *context)
{
   nk_bool selected = static_cast<nk_bool>(mSelected);
   nk_selectable_label(context, mText.c_str(), mTextAlignFlags, &selected);
   mSelected = static_cast<bool>(selected);
}

#pragma endregion

#pragma region Layout Row Implementations

void GuiLayoutRowDynamic::ChildRender(WindowBase *const window, nk_context *context)
{
   int height = GetHeight(window, context);

   nk_layout_row_dynamic(context, height, mControls.size());
   for (int i = 0; i < mControls.size(); i++)
   {
      mControls[i]->Render(window, context);
   }
}

void GuiLayoutRowStatic::ChildRender(WindowBase *const window, nk_context *context)
{
   int height = GetHeight(window, context);
   int width = mColWidth * window->GetContentScaleX();

   nk_layout_row_static(context, height, width, mControls.size());
   for (int i = 0; i < mControls.size(); i++)
   {
      mControls[i]->Render(window, context);
   }
}

void GuiLayoutRowDynamicGrid::ChildRender(WindowBase *const window, nk_context *context)
{
   if (mControls.size() > mScales.size())
   {
      throw std::runtime_error("More controls exist than scales.");
   }

   int height = GetHeight(window, context);
   nk_layout_row_begin(context, NK_DYNAMIC, height, mControls.size());

   // There's guaranteed to be one scale for every control.
   // There might be more scales set, but we can safely ignore those.
   for (int i = 0; i < mControls.size(); i++)
   {
      nk_layout_row_push(context, (float)*mScales[i]);
      mControls[i]->Render(window, context);
   }

   nk_layout_row_end(context);
}

void GuiLayoutRowStaticGrid::ChildRender(WindowBase *const window, nk_context *context)
{
   if (mControls.size() > mScales.size())
   {
      throw std::runtime_error("More controls exist than scales.");
   }

   // Min cols is ignored here.
   int height = GetHeight(window, context);
   nk_layout_row_begin(context, NK_STATIC, height, mControls.size());

   // There's guaranteed to be one scale for every control.
   // There might be more scales set, but we can safely ignore those.
   for (int i = 0; i < mControls.size(); i++)
   {
      int scale = static_cast<int>(*mScales[i]) * window->GetContentScaleX();
      nk_layout_row_push(context, scale);
      mControls[i]->Render(window, context);
   }

   nk_layout_row_end(context);
}

void GuiLayoutRowVariableGrid::ChildRender(WindowBase *const window, nk_context *context)
{
   if (mControls.size() > mScales.size())
   {
      throw std::runtime_error("More controls exist than scales.");
   }

   // Min cols is ignored here.
   int height = GetHeight(window, context);
   nk_layout_row_template_begin(context, height);

   for (int i = 0; i < mControls.size(); i++)
   {
      int scale = *mScales[i] * window->GetContentScaleX();
      nk_layout_row_template_push_variable(context, scale);
   }

   nk_layout_row_template_end(context);

   // There's guaranteed to be one scale for every control.
   // There might be more scales set, but we can safely ignore those.
   for (int i = 0; i < mControls.size(); i++)
   {
      mControls[i]->Render(window, context);
   }
}

bool GuiLayoutStaticSpace::AddChild(GuiControlBase *newControl)
{
   if (GuiLayoutRowBase::AddChild(newControl))
   {
      mWidths.push_back(&newControl->GetOrCreateAttribute<int64_t, AttrInt>((std::string)RootAttrName + "." +
                                                                            (std::string)WidthGridAttr));
      mHeights.push_back(&newControl->GetOrCreateAttribute<int64_t, AttrInt>((std::string)RootAttrName + "." +
                                                                             (std::string)HeightGridAttr));
      mPositionsX.push_back(&newControl->GetOrCreateAttribute<int64_t, AttrInt>((std::string)RootAttrName + "." +
                                                                                (std::string)PosXGridAttr));
      mPositionsY.push_back(&newControl->GetOrCreateAttribute<int64_t, AttrInt>((std::string)RootAttrName + "." +
                                                                                (std::string)PosYGridAttr));
   }

   return true;
}

void GuiLayoutStaticSpace::ChildRender(WindowBase *const window, nk_context *context)
{
   assert(!mAutoHeight);
   if (mControls.size() > mWidths.size())
   {
      throw std::runtime_error("More controls exist than scale and position attributes.");
   }

   // Min cols is ignored here.
   int height = GetHeight(window, context);
   nk_layout_space_begin(context, NK_STATIC, height, mControls.size());

   for (int i = 0; i < mControls.size(); i++)
   {
      nk_layout_space_push(
         context, nk_rect(*mPositionsX[i] * window->GetContentScaleX(), *mPositionsY[i] * window->GetContentScaleY(),
                          *mWidths[i] * window->GetContentScaleX(), *mHeights[i] * window->GetContentScaleY()));
      mControls[i]->Render(window, context);
   }

   nk_layout_space_end(context);
}

bool GuiLayoutDynamicSpace::AddChild(GuiControlBase *newControl)
{
   if (GuiLayoutRowBase::AddChild(newControl))
   {
      mWidths.push_back(&newControl->GetOrCreateAttribute<double, AttrReal>((std::string)RootAttrName + "." +
                                                                            (std::string)WidthGridAttr));
      mHeights.push_back(&newControl->GetOrCreateAttribute<double, AttrReal>((std::string)RootAttrName + "." +
                                                                             (std::string)HeightGridAttr));
      mPositionsX.push_back(&newControl->GetOrCreateAttribute<double, AttrReal>((std::string)RootAttrName + "." +
                                                                                (std::string)PosXGridAttr));
      mPositionsY.push_back(&newControl->GetOrCreateAttribute<double, AttrReal>((std::string)RootAttrName + "." +
                                                                                (std::string)PosYGridAttr));
   }

   return true;
}

void GuiLayoutDynamicSpace::ChildRender(WindowBase *const window, nk_context *context)
{
   assert(!mAutoHeight);
   if (mControls.size() > mWidths.size())
   {
      throw std::runtime_error("More controls exist than scale and position attributes.");
   }

   // Min cols is ignored here.
   int height = GetHeight(window, context);
   nk_layout_space_begin(context, NK_DYNAMIC, height, mControls.size());

   for (int i = 0; i < mControls.size(); i++)
   {
      nk_layout_space_push(context, nk_rect(*mPositionsX[i], *mPositionsY[i], *mWidths[i], *mHeights[i]));
      mControls[i]->Render(window, context);
   }

   nk_layout_space_end(context);
}

void GuiLayoutGroup::ChildRender(WindowBase *const window, nk_context *context)
{
   nk_flags flags = WinFlagsToNkWinFlags(mFlags);

   // The following flags are disallowed for groups.
   if ((flags & (NK_WINDOW_TITLE | NK_WINDOW_CLOSABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_MOVABLE |
                 NK_WINDOW_MINIMIZED | NK_WINDOW_CLOSED | NK_WINDOW_SCALABLE | NK_WINDOW_SCALE_LEFT)))
   {
      assert("Using disallowed flags on a group" && false);
   }

   flags |= (!mScrollable) ? NK_WINDOW_NO_SCROLLBAR : 0;
   flags |= (mBorder) ? NK_WINDOW_BORDER : 0;

   if (nk_group_begin_titled(context, mName.c_str(), mTitle.c_str(), flags))
   {
      for (int i = 0; i < mControls.size(); i++)
      {
         mControls[i]->Render(window, context);
      }

      nk_group_end(context);
   }
}

void GuiLayoutTreeNode::ChildRender(WindowBase *const window, nk_context *context)
{
   nk_collapse_states state = mInitiallyOpen ? nk_collapse_states::NK_MAXIMIZED : nk_collapse_states::NK_MINIMIZED;

   bool previousState = mExpanded;
   mExpanded = nk_tree_push_hashed(context, NK_TREE_NODE, mText.c_str(), state, mName.c_str(), mName.size(), 0);

   // We need to delay the expansion of the tree by a single frame to avoid scrollbar issues.
   if (mExpanded != previousState && !previousState)
   {
      nk_tree_pop(context);
      return;
   }

   if (mExpanded)
   {
      for (int i = 0; i < mControls.size(); i++)
      {
         mControls[i]->Render(window, context);
      }

      nk_tree_pop(context);
   }
}

void GuiLayoutTreeTab::ChildRender(WindowBase *const window, nk_context *context)
{
   nk_collapse_states state = mInitiallyOpen ? nk_collapse_states::NK_MAXIMIZED : nk_collapse_states::NK_MINIMIZED;

   bool previousState = mExpanded;
   mExpanded = nk_tree_push_hashed(context, NK_TREE_TAB, mText.c_str(), state, mName.c_str(), mName.size(), 0);

   // We need to delay the expansion of the tree by a single frame to avoid scrollbar stuff.
   if (mExpanded != previousState && !previousState)
   {
      nk_tree_pop(context);
      return;
   }

   if (mExpanded)
   {
      for (int i = 0; i < mControls.size(); i++)
      {
         mControls[i]->Render(window, context);
      }

      nk_tree_pop(context);
   }
}

#pragma endregion

#pragma region Menu

void GuiMenuBar::ChildRender(WindowBase *const window, nk_context *context)
{
   nk_menubar_begin(context);

   for (int i = 0; i < mControls.size(); i++)
   {
      mControls[i]->Render(window, context);
   }

   nk_menubar_end(context);
}

void GuiMenu::ChildRender(WindowBase *const window, nk_context *context)
{
   if (mImagePath.empty())
   {
      int width = mWidth * window->GetContentScaleX();
      int height = GetHeight(window, context);
      if (nk_menu_begin_label(context, mText.c_str(), mTextAlignFlags, nk_vec2(width, height)))
      {
         for (int i = 0; i < mControls.size(); i++)
         {
            mControls[i]->Render(window, context);
         }

         nk_menu_end(context);
      }
   }
}

void GuiMenuItem::ChildRender(WindowBase *const window, nk_context *context)
{
   nk_menu_item_label(context, mText.c_str(), mTextAlignFlags);
}

#pragma endregion
} // namespace wgui
