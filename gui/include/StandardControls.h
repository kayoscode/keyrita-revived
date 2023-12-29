#pragma once
#include <concepts>
#include <cassert>
#include <functional>
#include "App.h"

#include "Attributes.h"

struct nk_context;

namespace wgui
{
   class WindowBase;

   enum eControlType
   {
      Window,
      Widget,
      LayoutRow,
      Group,
      Tree,
      Menu,
   };

   /// <summary>
   /// Purely abstract class designed to give developers a tool to program a generic ui layout.
   /// When rendered to the screen, even control is rendered together. They should be grouped up and only drawn within
   /// the screen space they were allocated for.
   /// The back end of the gui renderer will handle this part!
   /// </summary>
   class GuiControlBase
   {
   public:
      GuiControlBase()
         : mAttributes(std::make_unique<AttributeSet>()) {}

      virtual void Init() {}
      virtual std::string GetLabel() = 0;

      /// <summary>
      /// Renders the control to the screen.
      /// </summary>
      /// <param name="window"></param>
      /// <param name="context"></param>
      virtual void Render(WindowBase* const window, nk_context* context) = 0;
      virtual eControlType GetControlType() const = 0;
      virtual bool AddChild(GuiControlBase* control) { return false; }

      virtual void ForEachChild(std::function<void(GuiControlBase* control, int index)>)
      {
         assert(false);
      }

      /// <summary>
      /// Whether elements can be placed inside it or not.
      /// </summary>
      /// <returns></returns>
      virtual bool SupportsChildren() const { return false; }

      bool AttributeExists(const std::string& attribute)
      {
         return mAttributes->AttributeExists(attribute);
      }

      attr_type_id_t GetAttributeType(const std::string& attribute)
      {
         return mAttributes->Get(attribute)->GetType();
      }

      template <typename T, typename Q>
      T& GetOrCreateAttribute(const std::string& attrName)
         requires std::is_base_of_v<CtrlAttribute, Q>
      {
         // Add a new scale property for the newly added control.
         if (!mAttributes->AttributeExists(attrName))
         {
            return mAttributes->Add<Q>(attrName)->GetRef();
         }

         if (!mAttributes->Get(attrName)->Is<Q>())
         {
            Application::Logger.warning("Overwriting previously defined attribute: '{str}' - incorrect type", attrName.c_str());
            mAttributes->Get(attrName)->SetType<Q>();
         }

         return mAttributes->Get(attrName)->As<Q>()->GetRef();
      }

      AttributeSet* const GetAttributes() { return mAttributes.get(); }

   protected:
      std::unique_ptr<AttributeSet> mAttributes;
      std::string mControlType;
   };

   class ChildSupportingGuiControlBase : public GuiControlBase
   {
   public:
      ChildSupportingGuiControlBase() : GuiControlBase() { }
      bool SupportsChildren() const override { return true; }

      virtual void Init() override
      {
         // Initialize all children, then init ourselves.
         for (int i = 0; i < mControls.size(); i++)
         {
            mControls[i]->Init();
         }

         SelfInit();
      }

      virtual void SelfInit() { }

      bool AddChild(GuiControlBase* newControl) override
      {
         mControls.push_back(newControl);
         return true;
      }

      virtual void ForEachChild(std::function<void(GuiControlBase* child, int index)> function) override
      {
         for (int i = 0; i < mControls.size(); i++)
         {
            function(mControls[i], i);
         }
      }

   protected:
      std::vector<GuiControlBase*> mControls;
   };

#pragma region Gui Widgets

   enum class eTextAlignmentFlags
   {
      Left = 0x01,
      Center = 0x02,
      Right = 0x04,
      Top = 0x08,
      VerticalCenter = 0x10,
      Bottom = 0x20,

      CenterLeft = VerticalCenter | Left,
      FullyCentered = VerticalCenter | Center,
      CenterRight = VerticalCenter | Right,
   };

   class GuiWidget : public GuiControlBase
   {
   public:
      eControlType GetControlType() const override
      {
         return eControlType::Widget;
      }
   };

   /// <summary>
   /// Fills the horizontal space with a horizontal separator line.
   /// </summary>
   class GuiHorizontalSeparator : public GuiWidget
   {
   public:
      static constexpr std::string_view ThicknessAttribute = "Thickness";

      GuiHorizontalSeparator() 
         : mThickness(mAttributes->Add<AttrInt>((std::string)ThicknessAttribute)->GetRef()),
           GuiWidget() 
      {
         mThickness = 1;
      }

      GuiHorizontalSeparator(float thickness)
         : GuiHorizontalSeparator()
      {
         mThickness = thickness;
      }

      void Render(WindowBase* const window, nk_context* context) override;

      std::string GetLabel() override { return "HorizontalSeparator"; }

   protected:
      int64_t& mThickness;
   };

   class GuiLabel : public GuiWidget
   {
   public:
      static constexpr std::string_view TextAlignAttr = "TextAlignment";
      static constexpr std::string_view TextAttr = "Text";

      GuiLabel()
         : GuiWidget(),
         mText(mAttributes->Add<AttrString>((std::string)TextAttr)->GetRef()),
         mTextAlignment(mAttributes->Add<AttrInt>((std::string)TextAlignAttr)->GetRef())
      {
         mText = "";
         mTextAlignment = static_cast<int64_t>(eTextAlignmentFlags::CenterLeft);
      }

      GuiLabel(const std::string& text, eTextAlignmentFlags alignment)
         : GuiLabel()
      {
         mText = text;
         mTextAlignment = static_cast<int64_t>(alignment);
      }

      void Render(WindowBase* const window, nk_context* context) override;

      std::string GetLabel() override { return "Label"; }

   protected:
      int64_t& mTextAlignment;
      std::string& mText;
   };

   class GuiCheckbox : public GuiLabel
   {
   public:
      static constexpr std::string_view CheckedAttr = "Checked";
      GuiCheckbox()
        :  mChecked(mAttributes->Add<AttrBool>((std::string)CheckedAttr)->GetRef()),
           GuiLabel()
      {
      }

      GuiCheckbox(const std::string& text, 
         eTextAlignmentFlags alignmentFlags = eTextAlignmentFlags::FullyCentered, bool checked = false)
         : GuiCheckbox()
      {
         mChecked = checked;
         mText = text;
         mTextAlignment = static_cast<int64_t>(alignmentFlags);
      }

      void Render(WindowBase* const window, nk_context* context) override;

      std::string GetLabel() override { return "Checkbox"; }
          
   protected:
      bool& mChecked;
   };

   class GuiButton : public GuiWidget
   {
   public:
      GuiButton()
         : mText(mAttributes->Add<AttrString>((std::string)GuiLabel::TextAttr)->GetRef())
      {
         mText = "";
      }

      GuiButton(const std::string& text, eTextAlignmentFlags alignmentFlags = eTextAlignmentFlags::FullyCentered)
         : GuiButton()
      {
         mText = text;
      }

      void Render(WindowBase* const window, nk_context* context) override;

      std::string GetLabel() override { return "Button"; }
   private:
      std::string& mText;
   };

   class GuiRadioButton : public GuiLabel
   {
   public:
      GuiRadioButton()
         : mButtonIndex(1), mRadioButtonSelection(nullptr), GuiLabel()
      {
      }

      void SetRadioButtonGroupSelection(int64_t* selection, int buttonIndex)
      {
         mRadioButtonSelection = selection;
         mButtonIndex = buttonIndex;
      }

      GuiRadioButton(const std::string& text,
         eTextAlignmentFlags alignmentFlags = eTextAlignmentFlags::FullyCentered, bool checked = false)
         : GuiRadioButton()
      {
         mText = text;
         mTextAlignment = static_cast<int64_t>(alignmentFlags);
      }

      void Render(WindowBase* const window, nk_context* context) override;

      std::string GetLabel() override { return "RadioButton"; }

   protected:
      int64_t* mRadioButtonSelection;
      int mButtonIndex;
   };

   /// <summary>
   /// All the radio buttons directly within this group will be controlled together.
   /// </summary>
   class GuiRadioButtonGroup : public ChildSupportingGuiControlBase
   {
   public:
      static constexpr std::string_view SelectedAttr = "Selection";

      GuiRadioButtonGroup()
         : mCurrentSelection(mAttributes->Add<AttrInt>((std::string)SelectedAttr)->GetRef()),
           ChildSupportingGuiControlBase()
      {
         mCurrentSelection = 1;
      }

      void SelfInit() override
      {
         mRadioButtons.clear();

         auto* radioBtns = &mRadioButtons;
         int64_t* currentSelection = &mCurrentSelection;
         int buttonIndex = 1;

         std::function<void(GuiControlBase*, int index)> func = 
            [&func, radioBtns, currentSelection, &buttonIndex](GuiControlBase* control, int index) -> void
            {
               GuiRadioButton* rb = dynamic_cast<GuiRadioButton*>(control);
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

      eControlType GetControlType() const override { return eControlType::Group; }

      void Render(WindowBase* const window, nk_context* context) override;

      std::string GetLabel() override { return "RadioButtonGroup"; }

   protected:
      std::vector<GuiRadioButton*> mRadioButtons;
      int64_t& mCurrentSelection;
   };

   class GuiComboboxItemBase : public GuiWidget
   {
   public:
      GuiComboboxItemBase()
         : mComboboxSelectedItem(nullptr),
         mItemHeight(nullptr)
      {
      }

      void SetComboboxSelectedItem(int64_t* selectedItem, int64_t* itemHeight, int itemIndex)
      {
         mComboboxSelectedItem = selectedItem;
         mItemIndex = itemIndex;
         mItemHeight = itemHeight;
      }

   protected:
      int64_t* mComboboxSelectedItem;
      int64_t* mItemHeight;
      int mItemIndex = 0;
   };

   class GuiComboboxItem : public GuiComboboxItemBase
   {
   public:
      static constexpr std::string_view TextAlignAttr = "TextAlignment";
      static constexpr std::string_view TextAttr = "Text";

      GuiComboboxItem()
         : GuiComboboxItemBase(),
         mText(mAttributes->Add<AttrString>((std::string)TextAttr)->GetRef()),
         mTextAlignment(mAttributes->Add<AttrInt>((std::string)TextAlignAttr)->GetRef())
      {
         mText = "";
         mTextAlignment = static_cast<int64_t>(eTextAlignmentFlags::CenterLeft);
      }

      GuiComboboxItem(const std::string& text, eTextAlignmentFlags alignment)
         : GuiComboboxItem()
      {
         mText = text;
         mTextAlignment = static_cast<int64_t>(alignment);
      }

      void Render(WindowBase* const window, nk_context* context) override;

      std::string GetLabel() override { return "ComboboxItem"; }

   protected:
      int64_t& mTextAlignment;
      std::string& mText;
   };

   class GuiCombobox : public ChildSupportingGuiControlBase
   {
   public:
      static constexpr std::string_view WidthAttr = "Width";
      static constexpr std::string_view HeightAttr = "Height";
      static constexpr std::string_view ItemHeightAttr = "ItemHeight";

      GuiCombobox()
         : mSelectedItem(mAttributes->Add<AttrInt>((std::string)GuiRadioButtonGroup::SelectedAttr)->GetRef()),
         mWidth(mAttributes->Add<AttrInt>((std::string)WidthAttr)->GetRef()),
         mHeight(mAttributes->Add<AttrInt>((std::string)HeightAttr)->GetRef()),
         mItemHeight(mAttributes->Add<AttrInt>((std::string)ItemHeightAttr)->GetRef())
      {
         mSelectedItem = 1;
         mWidth = 200;
         mHeight = 200;
         mItemHeight = 35;
      }

      GuiCombobox(int selectedItem, int width = 200, int height = 200, int itemHeight = 35)
         : GuiCombobox()
      {
         mSelectedItem = selectedItem;
         mWidth = width;
         mHeight = height;
         mItemHeight = 35;
      }

      eControlType GetControlType() const override { return eControlType::Group; }

      void SelfInit() override
      {
         mComboboxItems.clear();
         mComboboxTexts.clear();

         auto* items = &mComboboxItems;
         auto* texts = &mComboboxTexts;
         int64_t* currentSelection = &mSelectedItem;
         int64_t* itemHeight = &mItemHeight;
         int itemIndex = 1;

         std::function<void(GuiControlBase*, int index)> func =
            [&func, items, texts, currentSelection, itemHeight, &itemIndex](GuiControlBase* control, int index) -> void
            {
               GuiComboboxItem* cb = dynamic_cast<GuiComboboxItem*>(control);
               if (cb != nullptr)
               {
                  cb->SetComboboxSelectedItem(currentSelection, itemHeight, itemIndex);
                  itemIndex++;
                  items->push_back(cb);

                  texts->push_back(&cb->GetOrCreateAttribute<std::string, AttrString>((std::string)GuiComboboxItem::TextAttr));
               }

               if (control != nullptr && control->SupportsChildren())
               {
                  control->ForEachChild(func);
               }
            };

         func(this, 0);
      }

      void Render(WindowBase* const window, nk_context* context) override;
      std::string GetLabel() override { return "Combobox"; }

   protected:
      int64_t& mSelectedItem;
      int64_t& mWidth;
      int64_t& mHeight;
      int64_t& mItemHeight;

      std::vector<GuiComboboxItem*> mComboboxItems;
      std::vector<std::string*> mComboboxTexts;
   };

   class GuiSlider : public GuiWidget
   {
   public:
      static constexpr std::string_view ValueAttr = "Value";
      static constexpr std::string_view MaxValueAttr = "Max";
      static constexpr std::string_view MinValueAttr = "Min";
      static constexpr std::string_view StepAttr = "Step";
   };

   class GuiSliderInt : public GuiSlider
   {
   public:
      GuiSliderInt()
         : mValue(mAttributes->Add<AttrInt>((std::string)ValueAttr)->GetRef()),
           mMinValue(mAttributes->Add<AttrInt>((std::string)MinValueAttr)->GetRef()),
           mMaxValue(mAttributes->Add<AttrInt>((std::string)MaxValueAttr)->GetRef()),
           mStep(mAttributes->Add<AttrInt>((std::string)StepAttr)->GetRef())
      {
         mMinValue = 0;
         mMaxValue = 100;
         mValue = (mMinValue + mMaxValue) / 2;
         mStep = 1;
      }

      GuiSliderInt(int64_t min, int64_t max, int64_t value, int64_t step = 1)
         : GuiSliderInt()
      {
         mValue = value;
         mMinValue = min;
         mMaxValue = max;
         mStep = step;
      }

      void Render(WindowBase* const window, nk_context* context) override;
      std::string GetLabel() override { return "SliderInt"; }
      
   protected:
      int64_t& mValue;
      int64_t& mMinValue;
      int64_t& mMaxValue;
      int64_t& mStep;
   };

   class GuiSliderReal : public GuiSlider
   {
   public:
      GuiSliderReal()
         : mValue(mAttributes->Add<AttrReal>((std::string)ValueAttr)->GetRef()),
           mMinValue(mAttributes->Add<AttrReal>((std::string)MinValueAttr)->GetRef()),
           mMaxValue(mAttributes->Add<AttrReal>((std::string)MaxValueAttr)->GetRef()),
           mStep(mAttributes->Add<AttrReal>((std::string)StepAttr)->GetRef())
      {
         mMinValue = 0;
         mMaxValue = 1;
         mValue = (mMinValue + mMaxValue) / 2.0;
         mStep = 0.01;
      }

      GuiSliderReal(double min, double max, double value, double step = 1)
         : GuiSliderReal()
      {
         mValue = value;
         mMinValue = min;
         mMaxValue = max;
         mStep = step;
      }

      void Render(WindowBase* const window, nk_context* context) override;
      std::string GetLabel() override { return "SliderReal"; }

   protected:
      double& mValue;
      double& mMinValue;
      double& mMaxValue;
      double& mStep;
   };

   class GuiProgressBar : public GuiWidget
   {
   public:
      static constexpr std::string_view ValueAttr = "Value";
      static constexpr std::string_view MaxValueAttr = "Max";
      static constexpr std::string_view ModifableAttr = "Modifiable";

      GuiProgressBar()
         : mValue(mAttributes->Add<AttrInt>((std::string)ValueAttr)->GetRef()),
         mMaxValue(mAttributes->Add<AttrInt>((std::string)MaxValueAttr)->GetRef()),
         mModifiable(mAttributes->Add<AttrBool>((std::string)ModifableAttr)->GetRef())
      {
         mValue = 0;
         mMaxValue = 100;
         mModifiable = false;
      }

      GuiProgressBar(int64_t value, int64_t maxValue, bool modifiable = false)
         : GuiProgressBar()
      {
         mValue = value;
         mMaxValue = maxValue;
         mModifiable = modifiable;
      }

      void Render(WindowBase* const window, nk_context* context) override;
      std::string GetLabel() override { return "ProgressBar"; }

   protected:
      int64_t& mValue;
      int64_t& mMaxValue;
      bool& mModifiable;
   };

   class GuiInputInt : public GuiSliderInt
   {
   public:
      static constexpr std::string_view NameAttr = "Name";
      static constexpr std::string_view StepPerPxAttr = "StepPerPixel";
      
      GuiInputInt()
         : mName(mAttributes->Add<AttrString>((std::string)NameAttr)->GetRef()),
           mStepPerPx(mAttributes->Add<AttrReal>((std::string)StepPerPxAttr)->GetRef())
      {
         mName = "Unnamed";
         mStepPerPx = 1.0;
      }

      GuiInputInt(const std::string& name, int64_t min, int64_t max, int64_t value, int64_t step, double stepPerPixel)
         : GuiInputInt()
      {
         mMinValue = min;
         mMaxValue = max;
         mValue = value;
         mStep = step;
         mName = name;
         mStepPerPx = stepPerPixel;
      }

      void Render(WindowBase* const window, nk_context* context) override;
      std::string GetLabel() override { return "InputInt"; }

   protected:
      std::string& mName;
      double& mStepPerPx;
   };

   class GuiInputReal : public GuiSliderReal
   {
   public:
      static constexpr std::string_view NameAttr = "Name";
      static constexpr std::string_view StepPerPxAttr = "StepPerPixel";

      GuiInputReal()
         : mName(mAttributes->Add<AttrString>((std::string)NameAttr)->GetRef()),
         mStepPerPx(mAttributes->Add<AttrReal>((std::string)StepPerPxAttr)->GetRef())
      {
         mName = "Unnamed";
         mStepPerPx = 1.0;
      }

      GuiInputReal(const std::string& name, int64_t min, int64_t max, int64_t value, int64_t step, double stepPerPixel)
         : GuiInputReal()
      {
         mMinValue = min;
         mMaxValue = max;
         mValue = value;
         mStep = step;
         mName = name;
         mStepPerPx = stepPerPixel;
      }

      void Render(WindowBase* const window, nk_context* context) override;
      std::string GetLabel() override { return "InputReal"; }

   protected:
      std::string& mName;
      double& mStepPerPx;
   };

   class GuiSelectableLabel : public GuiLabel
   {
   public:
      static constexpr std::string_view SelectedAttr = "Selected";

      GuiSelectableLabel(bool selected = false)
         : mSelected(mAttributes->Add<AttrBool>((std::string)SelectedAttr)->GetRef()),
           GuiLabel()
      {
         mSelected = selected;
      }

      void Render(WindowBase* const window, nk_context* context) override;
      std::string GetLabel() override { return "SelectableLabel"; }

   protected:
      bool& mSelected;
   };

#pragma endregion

#pragma region Row Layouts

   class GuiLayoutRowBase : public ChildSupportingGuiControlBase
   {
   public:
      static constexpr std::string_view HeightAttr = "Height";

      GuiLayoutRowBase(int height)
         :GuiLayoutRowBase()
      {
         mHeight = height;
      }

      GuiLayoutRowBase()
         : ChildSupportingGuiControlBase(),
         mHeight(mAttributes->Add<AttrInt>((std::string)HeightAttr)->GetRef())
      {
         mHeight = 50;
      }

      eControlType GetControlType() const override
      {
         return eControlType::LayoutRow;
      }

   protected:
      int64_t& mHeight;
   };

   /// <summary>
   /// A grid of evenly spaced columns.
   /// </summary>
   class GuiLayoutRowDynamic : public GuiLayoutRowBase
   {
   public:
      GuiLayoutRowDynamic(int height) : GuiLayoutRowBase(height) { }
      GuiLayoutRowDynamic() : GuiLayoutRowBase() { }
      void Render(WindowBase* const window, nk_context* context) override;
      std::string GetLabel() override { return "DynamicRow"; }
   };

   /// <summary>
   /// A grid of fixed sized columns
   /// </summary>
   class GuiLayoutRowStatic : public GuiLayoutRowBase
   {
   public:
      static constexpr std::string_view ColWidthAttr = "Width";

      GuiLayoutRowStatic(int height, int colWidth)
         : GuiLayoutRowStatic()
      {
         mColWidth = colWidth;
         mHeight = height;
      }

      GuiLayoutRowStatic()
         : mColWidth(mAttributes->Add<AttrInt>((std::string)ColWidthAttr)->GetRef()),
         GuiLayoutRowBase()
      {
         mColWidth = 100;
      }

      void Render(WindowBase* const window, nk_context* context) override;
      std::string GetLabel() override { return "StaticRow"; }

   protected:
      int64_t& mColWidth;
   };

   /// <summary>
   /// A grid of widgets which can have percentage scaled widths.
   /// </summary>
   class GuiLayoutRowDynamicGrid : public GuiLayoutRowBase
   {
   public:
      static constexpr std::string_view WidthAttr = "Width";

      GuiLayoutRowDynamicGrid()
         : GuiLayoutRowBase()
      {
      }

      GuiLayoutRowDynamicGrid(int height) : GuiLayoutRowBase(height)
      {
      }

      bool AddChild(GuiControlBase* newControl) override
      {
         if (GuiLayoutRowBase::AddChild(newControl))
         {
            // Add a new scale property for the newly added control.
            std::string attrName = (std::string)WidthAttr + (std::to_string(mScales.size() + 1));
            mScales.push_back(&GetOrCreateAttribute<double, AttrReal>(attrName));
         }

         return true;
      }

      void Render(WindowBase* const window, nk_context* context) override;
      std::string GetLabel() override { return "DynamicGrid"; }

   protected:
      std::vector<double*> mScales;
   };

   /// <summary>
   /// A grid of widgets which can have constant sized widths.
   /// </summary>
   class GuiLayoutRowStaticGrid : public GuiLayoutRowBase
   {
   public:
      static constexpr std::string_view WidthAttr = "Width";

      GuiLayoutRowStaticGrid(int height) : GuiLayoutRowBase(height)
      {
      }

      GuiLayoutRowStaticGrid()
         : GuiLayoutRowBase() { }

      bool AddChild(GuiControlBase* newControl) override
      {
         if (GuiLayoutRowBase::AddChild(newControl))
         {
            std::string attrName = (std::string)WidthAttr + (std::to_string(mScales.size() + 1));
            mScales.push_back(&GetOrCreateAttribute<int64_t, AttrInt>(attrName));
         }

         return true;
      }

      void Render(WindowBase* const window, nk_context* context) override;
      std::string GetLabel() override { return "StaticGrid"; }

   protected:
      std::vector<int64_t*> mScales;
   };

   /// <summary>
   /// A combination of statically and dynamically spaced columns.
   /// </summary>
   class GuiLayoutRowVariableGrid : public GuiLayoutRowBase
   {
   public:
      GuiLayoutRowVariableGrid(int height) : GuiLayoutRowBase(height) { }
      GuiLayoutRowVariableGrid() : GuiLayoutRowBase() { }

      bool AddChild(GuiControlBase* newControl) override
      {
         if (GuiLayoutRowBase::AddChild(newControl))
         {
            // Add a new scale property for the newly added control.
            std::string attrName = "MinWidth" + (std::to_string(mScales.size() + 1));
            mScales.push_back(&GetOrCreateAttribute<int64_t, AttrInt>(attrName));
         }

         return true;
      }

      void Render(WindowBase* const window, nk_context* context) override;
      std::string GetLabel() override { return "VariableGrid"; }

   protected:
      std::vector<int64_t*> mScales;
   };

   class GuiLayoutSpace : public GuiLayoutRowBase
   {
   public:
      GuiLayoutSpace(int height) : GuiLayoutRowBase(height) { }
      GuiLayoutSpace() : GuiLayoutRowBase() { }

      static constexpr std::string_view RootAttrName = "Space";

      static constexpr std::string_view WidthGridAttr = "Width";
      static constexpr std::string_view HeightGridAttr = "Height";
      static constexpr std::string_view PosXGridAttr = "PosY";
      static constexpr std::string_view PosYGridAttr = "PosX";
   };

   /// <summary>
   /// Allows you to specify the height and width of columns in the grid (up to the max height)
   /// Statically spaced columns
   /// </summary>
   class GuiLayoutStaticSpace : public GuiLayoutSpace
   {
   public:
      GuiLayoutStaticSpace(int height) : GuiLayoutSpace(height) { }
      GuiLayoutStaticSpace() : GuiLayoutSpace() { }

      void Render(WindowBase* const window, nk_context* context) override;

      bool AddChild(GuiControlBase* newControl) override
      {
         if (GuiLayoutRowBase::AddChild(newControl))
         {
            mWidths.push_back(&newControl->GetOrCreateAttribute<int64_t, AttrInt>
               ((std::string)RootAttrName + "." + (std::string)WidthGridAttr));
            mHeights.push_back(&newControl->GetOrCreateAttribute<int64_t, AttrInt>
               ((std::string)RootAttrName + "." + (std::string)HeightGridAttr));
            mPositionsX.push_back(&newControl->GetOrCreateAttribute<int64_t, AttrInt>
               ((std::string)RootAttrName + "." + (std::string)PosXGridAttr));
            mPositionsY.push_back(&newControl->GetOrCreateAttribute<int64_t, AttrInt>
               ((std::string)RootAttrName + "." + (std::string)PosYGridAttr));
         }

         return true;
      }

      std::string GetLabel() override { return "StaticSpace"; }

   protected:
      /// <summary>
      /// Pointers to attributes of child elements.
      /// These attributes do not exist within this instance!!
      /// </summary>
      std::vector<int64_t*> mWidths;
      std::vector<int64_t*> mHeights;
      std::vector<int64_t*> mPositionsX;
      std::vector<int64_t*> mPositionsY;
   };

   /// <summary>
   /// Allows you to specify the height and width of columns in the grid (up to the max height)
   /// Dynamically spaced columns
   /// </summary>
   class GuiLayoutDynamicSpace : public GuiLayoutSpace
   {
   public:
      GuiLayoutDynamicSpace(int height) : GuiLayoutSpace(height) { }
      GuiLayoutDynamicSpace() : GuiLayoutSpace() { }

      void Render(WindowBase* const window, nk_context* context) override;

      bool AddChild(GuiControlBase* newControl) override
      {
         if (GuiLayoutRowBase::AddChild(newControl))
         {
            mWidths.push_back(&newControl->GetOrCreateAttribute<double, AttrReal>
               ((std::string)RootAttrName + "." + (std::string)WidthGridAttr));
            mHeights.push_back(&newControl->GetOrCreateAttribute<double, AttrReal>
               ((std::string)RootAttrName + "." + (std::string)HeightGridAttr));
            mPositionsX.push_back(&newControl->GetOrCreateAttribute<double, AttrReal>
               ((std::string)RootAttrName + "." + (std::string)PosXGridAttr));
            mPositionsY.push_back(&newControl->GetOrCreateAttribute<double, AttrReal>
               ((std::string)RootAttrName + "." + (std::string)PosYGridAttr));
         }

         return true;
      }

      std::string GetLabel() override { return "DynamicSpace"; }

   protected:
      /// <summary>
      /// Pointers to attributes of child elements.
      /// These attributes do not exist within this instance!!
      /// </summary>
      std::vector<double*> mWidths;
      std::vector<double*> mHeights;
      std::vector<double*> mPositionsX;
      std::vector<double*> mPositionsY;
   };

   class GuiLayoutGroup : public ChildSupportingGuiControlBase
   {
   public:
      static constexpr std::string_view TitleAttr = "Title";
      static constexpr std::string_view ScrollXAttr = "ScrollX";
      static constexpr std::string_view ScrollYAttr = "ScrollY";
      static constexpr std::string_view Scrollable = "Scrollable";

      GuiLayoutGroup()
         : mTitle(mAttributes->Add<AttrString>((std::string)TitleAttr)->GetRef()),
         mScrollable(mAttributes->Add<AttrBool>((std::string)Scrollable)->GetRef())
      {
         mTitle = "";
         mName = std::to_string(reinterpret_cast<int64_t>(this));
         mScrollable = false;
      }

      GuiLayoutGroup(const std::string& title, bool scrollable = false)
         : GuiLayoutGroup()
      {
         mTitle = title;
         mScrollable = scrollable;
      }

      eControlType GetControlType() const override { return eControlType::Group; }

      void Render(WindowBase* const window, nk_context* context) override;

      std::string GetLabel() override { return "Group"; }

   protected:
      std::string& mTitle;
      std::string mName;
      bool& mScrollable;
   };

   class GuiLayoutTreeBase : public ChildSupportingGuiControlBase
   {
   public:
      static constexpr std::string_view InitiallyOpenAttr = "InitiallyOpen";

      GuiLayoutTreeBase()
         : mInitiallyOpen(mAttributes->Add<AttrBool>((std::string)InitiallyOpenAttr)->GetRef()),
         mText(mAttributes->Add<AttrString>((std::string)GuiLabel::TextAttr)->GetRef())
      {
         mText = "";
         mInitiallyOpen = false;
         mName = std::to_string(reinterpret_cast<int64_t>(this));
      }

      GuiLayoutTreeBase(const std::string& text, bool initiallyOpen = false)
         : GuiLayoutTreeBase()
      {
         mInitiallyOpen = initiallyOpen;
         mText = text;
      }

      eControlType GetControlType() const override { return eControlType::Tree; }

   protected:
      bool& mInitiallyOpen;
      std::string& mText;
      std::string mName;
   };

   /// <summary>
   /// A tree with a highlighted header to mark collapsable ui sections.
   /// </summary>
   class GuiLayoutTreeNode : public GuiLayoutTreeBase
   {
   public:
      GuiLayoutTreeNode()
         : GuiLayoutTreeBase() { }
      GuiLayoutTreeNode(const std::string& text, bool initiallyOpen = false)
         : GuiLayoutTreeBase(text, initiallyOpen) { }

      void Render(WindowBase* const window, nk_context* context) override;
      std::string GetLabel() override { return "TreeNode"; }
   };

   /// <summary>
   /// A tree with a non-highlighted header to mark collapsable ui sections.
   /// </summary>
   class GuiLayoutTreeTab : public GuiLayoutTreeBase
   {
   public:
      GuiLayoutTreeTab()
         : GuiLayoutTreeBase() { }
      GuiLayoutTreeTab(const std::string& text, bool initiallyOpen = false)
         : GuiLayoutTreeBase(text, initiallyOpen) { }

      void Render(WindowBase* const window, nk_context* context) override;
      std::string GetLabel() override { return "TreeTab"; }
   };

#pragma endregion

#pragma region Menu

   class GuiMenuBar : public ChildSupportingGuiControlBase
   {
   public:
      GuiMenuBar() : ChildSupportingGuiControlBase() { }
      void Render(WindowBase* const window, nk_context* context) override;
      eControlType GetControlType() const override { return eControlType::Menu; }
      std::string GetLabel() override { return "MenuBar"; }
   };

   class GuiMenu : public ChildSupportingGuiControlBase
   {
   public:
      static constexpr std::string_view TextAlignAttr = "TextAlignment";
      static constexpr std::string_view TextAttr = "Text";
      static constexpr std::string_view ImagePathAttr = "Image";
      static constexpr std::string_view WidthAttr = "Width";
      static constexpr std::string_view HeightAttr = "Height";

      GuiMenu()
         : ChildSupportingGuiControlBase(),
         mTextAlignment(mAttributes->Add<AttrInt>((std::string)TextAlignAttr)->GetRef()),
         mText(mAttributes->Add<AttrString>((std::string)TextAttr)->GetRef()),
         mImagePath(mAttributes->Add<AttrString>((std::string)ImagePathAttr)->GetRef()),
         mWidth(mAttributes->Add<AttrInt>((std::string)WidthAttr)->GetRef()),
         mHeight(mAttributes->Add<AttrInt>((std::string)HeightAttr)->GetRef())
      {
         mTextAlignment = static_cast<int64_t>(eTextAlignmentFlags::FullyCentered);
         mText = "";
         mImagePath = "";

         mWidth = 100;
         mHeight = 40;
      }

      GuiMenu(const std::string& text, const eTextAlignmentFlags alignment, 
         int height, int width,
         const std::string& imagePath = "")
         : GuiMenu()
      {
         mTextAlignment = static_cast<int64_t>(alignment);
         mText = text;
         mImagePath = imagePath;

         mWidth = width;
         mHeight = height;
      }

      void Render(WindowBase* const window, nk_context* context) override;

      eControlType GetControlType() const override { return eControlType::Menu; }
      bool SupportsChildren() const override { return true; }

      std::string GetLabel() override { return "Menu"; }

   protected:
      int64_t& mTextAlignment;
      std::string& mText;
      std::string& mImagePath;

      int64_t& mWidth, & mHeight;
   };

   class GuiMenuItem : public GuiWidget
   {
   public:
      static constexpr std::string_view TextAlignAttr = "TextAlignment";
      static constexpr std::string_view TextAttr = "Text";
      static constexpr std::string_view ImagePathAttr = "Image";

      GuiMenuItem()
         : GuiWidget(),
         mTextAlignment(mAttributes->Add<AttrInt>((std::string)TextAlignAttr)->GetRef()),
         mText(mAttributes->Add<AttrString>((std::string)TextAttr)->GetRef()),
         mImagePath(mAttributes->Add<AttrString>((std::string)ImagePathAttr)->GetRef())
      {
         mTextAlignment = static_cast<int64_t>(eTextAlignmentFlags::CenterLeft);
         mText = "";
         mImagePath = "";
      }

      GuiMenuItem(const std::string& text, const eTextAlignmentFlags alignment, 
         int height, int width,
         const std::string& imagePath = "")
         : GuiMenuItem()
      {
         mTextAlignment = static_cast<int64_t>(alignment);
         mText = text;
         mImagePath = imagePath;
      }

      void Render(WindowBase* const window, nk_context* context) override;
      eControlType GetControlType() const override { return eControlType::Widget; }
      std::string GetLabel() override { return "MenuItem"; }

   protected:
      int64_t& mTextAlignment;
      std::string& mText;
      std::string& mImagePath;
   };

   /// <summary>
   /// Class which owns its own controls.
   /// Can be parsed from an object, or created raw via the SelfInit method.
   /// </summary>
   class GuiAbstractControl : public ChildSupportingGuiControlBase
   {
   public:
      GuiAbstractControl()
         : mOwnedControls() { }

      eControlType GetControlType() const override { return eControlType::Group; }

   protected:
      // All controls created by this class must be owned. Store them here.
      // All children must get raw pointers to objects stored and owned here.
      std::vector<std::unique_ptr<GuiControlBase>> mOwnedControls;
   };

#pragma endregion

   /// <summary>
   /// Represents a window on the Gui. This can hold widgets to menus!
   /// </summary>
   class GuiLayoutWindow : public ChildSupportingGuiControlBase
   {
   public:
      static constexpr std::string_view XPosAttr = "XPos";
      static constexpr std::string_view YPosAttr = "YPos";
      static constexpr std::string_view WidthAttr = "Width";
      static constexpr std::string_view HeightAttr = "Height";
      static constexpr std::string_view TitleAttr = "Title";
      static constexpr std::string_view TrackParentAttr = "TrackWinScale";

      GuiLayoutWindow()
         : ChildSupportingGuiControlBase(),
         mPosX(mAttributes->Add<AttrInt>((std::string)XPosAttr)->GetRef()),
         mPosY(mAttributes->Add<AttrInt>((std::string)YPosAttr)->GetRef()),
         mWidth(mAttributes->Add<AttrInt>((std::string)WidthAttr)->GetRef()),
         mHeight(mAttributes->Add<AttrInt>((std::string)HeightAttr)->GetRef()),
         mTitle(mAttributes->Add<AttrString>((std::string)TitleAttr)->GetRef()),
         mTrackParentWindowSize(mAttributes->Add<AttrBool>((std::string)TrackParentAttr)->GetRef())
      {
         // Set to defaults.
         mPosX = 0;
         mPosY = 0;
         mWidth = 100;
         mHeight = 100;
         mTitle = "Untitled";

         mWindowName = std::to_string(reinterpret_cast<int64_t>(this));
      }

      GuiLayoutWindow(const std::string& name,
         int posX, int posY, int width, int height,
         const std::string& title)
         : GuiLayoutWindow()
      {
         mPosX = posX;
         mPosY = posY;
         mWidth = width;
         mHeight = height;
         mTitle = title;
      }

      void Render(WindowBase* const window, nk_context* context) override;

      eControlType GetControlType() const override
      {
         return eControlType::Window;
      }

      std::string GetLabel() override { return "Window"; }

   protected:
      int64_t& mPosX;
      int64_t& mPosY;

      int64_t& mWidth;
      int64_t& mHeight;

      std::string& mTitle;
      bool& mTrackParentWindowSize;
      std::string mWindowName;
   };
}
