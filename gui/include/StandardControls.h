#pragma once

#include "Attributes.h"
#include <concepts>

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

      /// <summary>
      /// Renders the control to the screen.
      /// </summary>
      /// <param name="window"></param>
      /// <param name="context"></param>
      virtual void Render(WindowBase* const window, nk_context* context) = 0;
      virtual eControlType GetControlType() const = 0;
      virtual bool AddChild(GuiControlBase* control) { return false; }

      /// <summary>
      /// Whether elements can be placed inside it or not.
      /// </summary>
      /// <returns></returns>
      virtual bool SupportsChildren() const { return false; }

      bool AttributeExists(const std::string& attribute)
      {
         return mAttributes->AttributeExists(attribute);
      }

      eAttributeType GetAttributeType(const std::string& attribute)
      {
         return mAttributes->Get(attribute)->GetType();
      }

      void SetAttributeInt(const std::string& attributeName, int64_t value)
      {
         if (!AttributeExists(attributeName))
         {
            mAttributes->Add(attributeName, eAttributeType::Int);
         }

         mAttributes->Get(attributeName)->As<AttrInt>()->Set(value);
      }

      void SetAttributeReal(const std::string& attributeName, double value)
      {
         if (!AttributeExists(attributeName))
         {
            mAttributes->Add(attributeName, eAttributeType::Real);
         }

         mAttributes->Get(attributeName)->As<AttrReal>()->Set(value);
      }

      void SetAttributeString(const std::string& attributeName, const std::string& value)
      {
         if (!AttributeExists(attributeName))
         {
            mAttributes->Add(attributeName, eAttributeType::String);
         }

         mAttributes->Get(attributeName)->As<AttrString>()->Set(value);
      }

      int64_t& GetOrCreateAttributeInt(const std::string& attrName)
      {
         return GetOrChooseAttributeBase<int64_t, AttrInt>(attrName, eAttributeType::Int);
      }

      double& GetOrCreateAttributeReal(const std::string& attrName)
      {
         return GetOrChooseAttributeBase<double, AttrReal>(attrName, eAttributeType::Real);
      }

      std::string& GetOrCreateAttributeString(const std::string& attrName)
      {
         return GetOrChooseAttributeBase<std::string, AttrString>(attrName, eAttributeType::String);
      }

      int64_t GetAttributeInt(const std::string& attributeName, int64_t value)
      {
         return mAttributes->Get(attributeName)->As<AttrInt>()->Get();
      }

      double GetAttributeReal(const std::string& attributeName, double value)
      {
         return mAttributes->Get(attributeName)->As<AttrReal>()->Get();
      }

      std::string GetAttributeString(const std::string& attributeName, const std::string& value)
      {
         return mAttributes->Get(attributeName)->As<AttrString>()->Get();
      }

      AttributeSet* const GetAttributes() { return mAttributes.get(); }

   protected:
      std::unique_ptr<AttributeSet> mAttributes;

   private:
      template <typename T, typename Q>
      T& GetOrChooseAttributeBase(const std::string& attrName, eAttributeType type)
         requires (std::is_same_v<T, int64_t> || std::is_same_v<T, double> || std::is_same_v<T, std::string>) &&
      (std::is_convertible_v<Q, CtrlAttribute>)
      {
         // Add a new scale property for the newly added control.
         if (!mAttributes->AttributeExists(attrName))
         {
            return mAttributes->Add(attrName, type)->As<Q>()->GetRef();
         }

         if (GetAttributeType(attrName) != type)
         {
            // TODO: warning case here.
            mAttributes->Get(attrName)->SetType(type);
         }

         return mAttributes->Get(attrName)->As<Q>()->GetRef();
      }
   };

   class ChildSupportingGuiControlBase : public GuiControlBase
   {
   public:
      ChildSupportingGuiControlBase() : GuiControlBase() { }
      bool SupportsChildren() const override { return true; }

      bool AddChild(GuiControlBase* newControl) override
      {
         mControls.push_back(newControl);
         return true;
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

      CenterLeft = Center | Left,
      FullyCentered = VerticalCenter | Center,
      CenterRight = Center | Right,
   };

   class GuiWidget : public GuiControlBase
   {
   public:
      eControlType GetControlType() const override
      {
         return eControlType::Widget;
      }
   };

   class GuiLabel : public GuiWidget
   {
   public:
      static constexpr std::string_view TextAlignAttr = "TextAlignment";
      static constexpr std::string_view TextAttr = "Text";

      GuiLabel()
         : GuiWidget(),
         mText(mAttributes->Add((std::string)TextAttr, eAttributeType::String)->As<AttrString>()->GetRef()),
         mTextAlignment(mAttributes->Add((std::string)TextAlignAttr, eAttributeType::Int)->As<AttrInt>()->GetRef())
      {
         mText = "";
         mTextAlignment = static_cast<int64_t>(eTextAlignmentFlags::FullyCentered);
      }

      GuiLabel(const std::string& text, eTextAlignmentFlags alignment)
         : GuiLabel()
      {
         mText = text;
         mTextAlignment = static_cast<int64_t>(alignment);
      }

      void Render(WindowBase* const window, nk_context* context) override;

   private:
      int64_t& mTextAlignment;
      std::string& mText;
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
         mHeight(mAttributes->Add((std::string)HeightAttr, eAttributeType::Int)->As<AttrInt>()->GetRef())
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
         : mColWidth(mAttributes->Add((std::string)ColWidthAttr, eAttributeType::Int)->As<AttrInt>()->GetRef()),
         GuiLayoutRowBase()
      {
         mColWidth = 100;
      }

      void Render(WindowBase* const window, nk_context* context) override;

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
            mScales.push_back(&GetOrCreateAttributeReal(attrName));
         }

         return true;
      }

      void Render(WindowBase* const window, nk_context* context) override;

   private:
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
            mScales.push_back(&GetOrCreateAttributeInt(attrName));
         }

         return true;
      }

      void Render(WindowBase* const window, nk_context* context) override;

   private:
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
            mScales.push_back(&GetOrCreateAttributeInt(attrName));
         }

         return true;
      }

      void Render(WindowBase* const window, nk_context* context) override;

   private:
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

      void GetBounds(nk_context* context, int& posX, int& posY, int& width, int& height);
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
            mWidths.push_back(&newControl->GetOrCreateAttributeInt
               ((std::string)RootAttrName + "." + (std::string)WidthGridAttr));
            mHeights.push_back(&newControl->GetOrCreateAttributeInt
               ((std::string)RootAttrName + "." + (std::string)HeightGridAttr));
            mPositionsX.push_back(&newControl->GetOrCreateAttributeInt
               ((std::string)RootAttrName + "." + (std::string)PosXGridAttr));
            mPositionsY.push_back(&newControl->GetOrCreateAttributeInt
               ((std::string)RootAttrName + "." + (std::string)PosYGridAttr));
         }

         return true;
      }

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
            mWidths.push_back(&newControl->GetOrCreateAttributeReal
               ((std::string)RootAttrName + "." + (std::string)WidthGridAttr));
            mHeights.push_back(&newControl->GetOrCreateAttributeReal
               ((std::string)RootAttrName + "." + (std::string)HeightGridAttr));
            mPositionsX.push_back(&newControl->GetOrCreateAttributeReal
               ((std::string)RootAttrName + "." + (std::string)PosXGridAttr));
            mPositionsY.push_back(&newControl->GetOrCreateAttributeReal
               ((std::string)RootAttrName + "." + (std::string)PosYGridAttr));
         }

         return true;
      }

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

   class GuiLayoutGroup : public GuiControlBase
   {
   public:
   };

   class GuiTree : public GuiControlBase
   {
   public:
   };

#pragma endregion

#pragma region Menu

   class GuiMenuBar : public ChildSupportingGuiControlBase
   {
   public:
      GuiMenuBar() : ChildSupportingGuiControlBase() { }
      void Render(WindowBase* const window, nk_context* context) override;
      eControlType GetControlType() const override { return eControlType::Menu; }
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
         mTextAlignment(mAttributes->Add((std::string)TextAlignAttr, eAttributeType::Int)->As<AttrInt>()->GetRef()),
         mText(mAttributes->Add((std::string)TextAttr, eAttributeType::String)->As<AttrString>()->GetRef()),
         mImagePath(mAttributes->Add((std::string)ImagePathAttr, eAttributeType::String)->As<AttrString>()->GetRef()),
         mWidth(mAttributes->Add((std::string)WidthAttr, eAttributeType::Int)->As<AttrInt>()->GetRef()),
         mHeight(mAttributes->Add((std::string)HeightAttr, eAttributeType::Int)->As<AttrInt>()->GetRef())
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

   private:
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
         mTextAlignment(mAttributes->Add((std::string)TextAlignAttr, eAttributeType::Int)->As<AttrInt>()->GetRef()),
         mText(mAttributes->Add((std::string)TextAttr, eAttributeType::String)->As<AttrString>()->GetRef()),
         mImagePath(mAttributes->Add((std::string)ImagePathAttr, eAttributeType::String)->As<AttrString>()->GetRef())
      {
         mTextAlignment = static_cast<int64_t>(eTextAlignmentFlags::FullyCentered);
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

   private:
      int64_t& mTextAlignment;
      std::string& mText;
      std::string& mImagePath;
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
      static constexpr std::string_view NameAttr = "Name";
      static constexpr std::string_view TitleAttr = "Title";

      GuiLayoutWindow()
         : ChildSupportingGuiControlBase(),
         mPosX(mAttributes->Add((std::string)XPosAttr, eAttributeType::Int)->As<AttrInt>()->GetRef()),
         mPosY(mAttributes->Add((std::string)YPosAttr, eAttributeType::Int)->As<AttrInt>()->GetRef()),
         mWidth(mAttributes->Add((std::string)WidthAttr, eAttributeType::Int)->As<AttrInt>()->GetRef()),
         mHeight(mAttributes->Add((std::string)HeightAttr, eAttributeType::Int)->As<AttrInt>()->GetRef()),
         mName(mAttributes->Add((std::string)NameAttr, eAttributeType::String)->As<AttrString>()->GetRef()),
         mTitle(mAttributes->Add((std::string)TitleAttr, eAttributeType::String)->As<AttrString>()->GetRef())
      {
         // Set to defaults.
         mPosX = 0;
         mPosY = 0;
         mWidth = 100;
         mHeight = 100;
         mName = "Unnamed";
         mTitle = "Untitled";
      }

      GuiLayoutWindow(const std::string& name,
         int posX, int posY, int width, int height,
         const std::string& title = "")
         : GuiLayoutWindow()
      {
         // TODO: do defaults with configuration files!!
         mPosX = posX;
         mPosY = posY;
         mWidth = width;
         mHeight = height;
         mName = name;
         mTitle = title;
      }

      void Render(WindowBase* const window, nk_context* context) override;

      eControlType GetControlType() const override
      {
         return eControlType::Window;
      }

   private:
      int64_t& mPosX;
      int64_t& mPosY;

      int64_t& mWidth;
      int64_t& mHeight;

      std::string& mName;
      std::string& mTitle;
   };
}
