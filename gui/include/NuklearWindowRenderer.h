#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <exception>
#include <map>
#include <functional>
#include <algorithm>
#include <stdint.h>

#include "pugixml.hpp"

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

   enum class eAttributeType
   {
      Int,
      Real,
      String
   };

   struct CaseInsensitiveStrCompare
   {
      bool operator()(const std::string& a, const std::string& b) const
      {
         return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), 
            [](char a, char b) 
            { 
               return std::tolower(a) < std::tolower(b); 
            });
      }
   };

#pragma region Attributes

   class CtrlAttribute
   {
   public:
      CtrlAttribute(eAttributeType type)
         : mType(type) { }

   private:
      eAttributeType mType;
   };

   class AttrInt : public CtrlAttribute
   {
   public:
      AttrInt()
         : mValue(0), CtrlAttribute(eAttributeType::Int) {}

      int64_t Get() const { return mValue; }
      operator int64_t() const { return mValue; }

      AttrInt Set(int64_t value)
      {
         mValue = value;
         return *this;
      }

      AttrInt operator = (int64_t value)
      {
         mValue = value;
         return *this;
      }

      int64_t& GetRef()
      {
         return mValue;
      }

   private:
      int64_t mValue;
   };

   class AttrReal : public CtrlAttribute
   {
   public:
      AttrReal()
         : mValue(0.0), CtrlAttribute(eAttributeType::Real) {}

      double Get() const { return mValue; }
      operator double() const { return mValue; }

      AttrReal Set(double value)
      {
         mValue = value;
         return *this;
      }

      AttrReal operator = (double value)
      {
         mValue = value;
         return *this;
      }

      double& GetRef()
      {
         return mValue;
      }

   private:
      double mValue;
   };

   class AttrString : public CtrlAttribute
   {
   public:
      AttrString()
           : CtrlAttribute(eAttributeType::String) {}

      std::string Get() const { return mValue; }
      operator std::string() const { return mValue; }

      AttrString Set(const std::string& value)
      {
         mValue = value;
         return *this;
      }

      AttrString operator = (const std::string& value)
      {
         mValue = value;
         return *this;
      }

      std::string& GetRef()
      {
         return mValue;
      }

   private:
      std::string mValue;
   };

   class Attribute
   {
   public:
      Attribute(eAttributeType type)
      {
         SetType(type);
      }

      eAttributeType GetType() { return mType; }

      template<class T>
      T* As()
      {
         if (!mValue)
         {
            throw std::exception("Attribute not initialized to a value");
         }

         return reinterpret_cast<T*>(mValue.get());
      }

      template<class T>
      T* const As() const
      {
         if (!mValue)
         {
            throw std::exception("Attribute not initialized to a value");
         }

         return reinterpret_cast<T* const>(mValue.get());
      }

      void SetType(eAttributeType type)
      {
         mType = type;

         switch (type)
         {
         case eAttributeType::Int:
            mValue = std::make_unique<AttrInt>();
            break;
         case eAttributeType::Real:
            mValue = std::make_unique<AttrReal>();
            break;
         case eAttributeType::String:
            mValue = std::make_unique<AttrString>();
            break;
         }
      }

   private:
      std::unique_ptr<CtrlAttribute> mValue;
      eAttributeType mType;
   };

   class AttributeSet
   {
   public:
      AttributeSet() : mAttributes() { }

      Attribute* operator[](const std::string& name) const
      {
         auto attr = mAttributes.find(name);

         if (attr == mAttributes.end())
         {
            throw std::out_of_range("Attribute with the given name was not found.");
         }

         return attr->second.get();
      }

      Attribute* Get(const std::string& name) const
      {
         return (*this)[name];
      }

      Attribute* Add(const std::string& name, eAttributeType type)
      {
         auto attr = mAttributes.find(name);

         if (attr != mAttributes.end())
         {
            throw std::out_of_range("Attribute with given name already exists.");
         }

         mAttributes.emplace(name, std::make_unique<Attribute>(type));
         return mAttributes[name].get();
      }

      bool AttributeExists(const std::string& name)
      {
         return mAttributes.find(name) != mAttributes.end();
      }

   private:
      std::map<std::string, 
               std::unique_ptr<Attribute>, 
               CaseInsensitiveStrCompare> mAttributes;
   };

#pragma endregion

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

      int64_t* GetOrChooseAttributeInt(const std::string& attrName) 
      {
         return GetOrChooseAttributeBase<int64_t, AttrInt>(attrName, eAttributeType::Int);
      }

      double* GetOrChooseAttributeDouble(const std::string& attrName)
      {
         return GetOrChooseAttributeBase<double, AttrReal>(attrName, eAttributeType::Real);
      }

      std::string* GetOrChooseAttributeString(const std::string& attrName)
      {
         return GetOrChooseAttributeBase<std::string, AttrString>(attrName, eAttributeType::String);
      }

   private:
      template <typename T, typename Q>
      T* GetOrChooseAttributeBase(const std::string& attrName, eAttributeType type)
         requires (std::is_same_v<T, int64_t> || std::is_same_v<T, double> || std::is_same_v<T, std::string>) &&
      (std::is_convertible_v<Q, CtrlAttribute>)
      {
         // Add a new scale property for the newly added control.
         if (!mAttributes->AttributeExists(attrName))
         {
            return &mAttributes->Add(attrName, type)->As<Q>()->GetRef();
         }

         if (GetAttributeType(attrName) != type)
         {
            // TODO: warning case here.
            mAttributes->Get(attrName)->SetType(type);
         }

         return &mAttributes->Get(attrName)->As<Q>()->GetRef();
      }
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

   class GuiLayoutRowBase : public GuiControlBase
   {
   public:
      static constexpr std::string_view HeightAttr = "Height";

      GuiLayoutRowBase(int height)
         :GuiLayoutRowBase()
      {
         mHeight = height;
      }

      GuiLayoutRowBase()
         : GuiControlBase(),
         mHeight(mAttributes->Add((std::string)HeightAttr, eAttributeType::Int)->As<AttrInt>()->GetRef())
      {
         mHeight = 50;
      }

      eControlType GetControlType() const override
      {
         return eControlType::LayoutRow;
      }

      bool SupportsChildren() const override { return true; }

      bool AddChild(GuiControlBase* newControl) override
      {
         // Make sure it's not another layout row. These cannot be nested.
         if (!(newControl->GetControlType() != eControlType::LayoutRow &&
            newControl->GetControlType() != eControlType::Menu))
         {
            throw std::runtime_error("Control does not support child elements of this type.");
         }

         mControls.push_back(newControl);

         return true;
      }

   protected:
      inline void RenderControls(WindowBase* const window, nk_context* context)
      {
         for (const auto& control : mControls)
         {
            control->Render(window, context);
         }
      }

      std::vector<GuiControlBase*> mControls;
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
      static constexpr std::string_view ColWidthAttr = "ColWidth";

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
            mScales.push_back(GetOrChooseAttributeDouble(attrName));
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
            mScales.push_back(GetOrChooseAttributeInt(attrName));
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
            mScales.push_back(GetOrChooseAttributeInt(attrName));
         }

         return true;
      }

      void Render(WindowBase* const window, nk_context* context) override;

   private:
      std::vector<int64_t*> mScales;
   };

   /// <summary>
   /// Allows you to specify the height and width of columns in the grid (up to the max height)
   /// Statically spaced columns
   /// </summary>
   class GuiLayoutStaticSpace : public GuiLayoutRowBase
   {
   public:
      GuiLayoutStaticSpace(int height) : GuiLayoutRowBase(height) { }
      GuiLayoutStaticSpace() : GuiLayoutRowBase() { }

      void Render(WindowBase* const window, nk_context* context) override;

      bool AddChild(GuiControlBase* newControl) override
      {
         if (GuiLayoutRowBase::AddChild(newControl))
         {
            //std::string attrName = "MinWidth" + (std::to_string(mScales.size() + 1));
         }

         return true;
      }

   private:
      std::vector<int64_t*> mWidths;
      std::vector<int64_t*> mHeights;
   };

   /// <summary>
   /// Allows you to specify the height and width of columns in the grid (up to the max height)
   /// Dynamically spaced columns
   /// </summary>
   class GuiLayoutDynamicSpace : public GuiLayoutRowBase
   {
   public:
      GuiLayoutDynamicSpace(int height) : GuiLayoutRowBase(height) { }
      GuiLayoutDynamicSpace() : GuiLayoutRowBase() { }
   };

#pragma endregion

   class GuiLayoutGroup : public GuiControlBase
   {
   public:
   };

   class GuiMenu : public GuiControlBase
   {
   public:
   };

   class GuiTree : public GuiControlBase
   {
   public:
   };

   /// <summary>
   /// Represents a window on the Gui. This can hold widgets to menus!
   /// </summary>
   class GuiLayoutWindow : public GuiControlBase
   {
   public:
      static constexpr std::string_view XPosAttr = "XPos";
      static constexpr std::string_view YPosAttr = "YPos";
      static constexpr std::string_view WidthAttr = "Width";
      static constexpr std::string_view HeightAttr = "Height";
      static constexpr std::string_view NameAttr = "Name";
      static constexpr std::string_view TitleAttr = "Title";

      GuiLayoutWindow()
         : GuiControlBase(),
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

      bool AddChild(GuiControlBase* control) override
      {
         mControls.push_back(control);
         return true;
      }

      void Render(WindowBase* const window, nk_context* context) override;

      eControlType GetControlType() const override
      {
         return eControlType::Window;
      }

      bool SupportsChildren() const { return true; }

   private:
      int64_t& mPosX;
      int64_t& mPosY;

      int64_t& mWidth;
      int64_t& mHeight;

      std::string& mName;
      std::string& mTitle;

      std::vector<GuiControlBase*> mControls;
   };

   /// <summary>
   /// Base class for all nuklear window render handlers.
   /// The implementation of this class can be as simple as just drawing a textbox or even doing nothing
   /// and it can be as complex as holding a structured list of controls and rendering them and updating them.
   /// It can be more complex than that!
   /// 
   /// This class is designed to be a layer above the game loop and allow developers to abstract
   /// their GUI rendering logic from the rest of their application's code.
   /// There is no implementation for this class, just a header file for clients to implement in their own way.
   /// 
   /// This class needs:
   /// 1. A callback to the main render process
   /// 2. A callback to the initialization process
   /// 3. A callback to the render cleanup process (moving to the next frame/etc/)
   /// 4. A pointer to a const GlfwWindow
   /// 5. Overridable callbacks to all windows events.
   /// 6. The rest is up to the implementer!
   /// </summary>
   class WindowRenderer
   {
   public:
      virtual void RenderStart(WindowBase* const window, nk_context* context) = 0;
      virtual void Render(WindowBase* const window, nk_context* context) = 0;
      virtual void RenderFinish(WindowBase* const window, nk_context* context) = 0;

   private:
   };

   class GuiControlFactory
   {
   public:
      virtual std::unique_ptr<GuiControlBase> CreateControl(const std::string& controlType) = 0;
   };

   /// <summary>
   /// Renders the gui based on the controls and layouts given by the developer.
   /// </summary>
   class WindowRendererGui : public WindowRenderer
   {
   public:
      WindowRendererGui()
      {
      }

      void RenderStart(WindowBase* const window, nk_context* context) override;
      void Render(WindowBase* const window, nk_context* context) override;
      void RenderFinish(WindowBase* const window, nk_context* delta) override;

      bool AddChild(GuiControlBase* window)
      {
         mWindows.push_back(window);
         return true;
      }

   private:
      std::vector<GuiControlBase*> mWindows;
   };

   /// <summary>
   /// A window renderer that loads its controls from an XML file.
   /// </summary>
   class XmlLoadedWindowRenderer : public WindowRendererGui
   {
   private:
      class StandardControlFactory : public GuiControlFactory
      {
      public:
         std::unique_ptr<GuiControlBase> CreateControl(const std::string& controlType);
      };

   public:
      XmlLoadedWindowRenderer()
      {
         AddControlFactory<StandardControlFactory>();
      }

      bool ConstructLayoutFromXml(const std::string& xml);
      bool ConstructLayoutFromXmlFile(const std::string& fileName);

      template <class T>
      void AddControlFactory() requires std::is_convertible_v<T, StandardControlFactory>
      {
         mControlFactories.push_back(std::make_unique<T>());
      }

      void AddControl(std::unique_ptr<GuiControlBase> window)
      {
         GuiControlBase* pWin = window.get();
         mOwnedControls.push_back(std::move(window));
         WindowRendererGui::AddChild(pWin);
      }

   private:
      std::vector<std::unique_ptr<GuiControlBase>> mOwnedControls;
      std::vector<std::unique_ptr<GuiControlFactory>> mControlFactories;

      void ConstructWindows(const pugi::xml_object_range<pugi::xml_node_iterator>& document);
      void ConstructComponents(const pugi::xml_object_range<pugi::xml_node_iterator>& document,
         GuiControlBase* parent);

      std::unique_ptr<GuiControlBase> CreateControl(const std::string& controlName)
      {
         for (const auto& factory : mControlFactories)
         {
            std::unique_ptr<GuiControlBase> newControl = factory->CreateControl(controlName);

            if (newControl != nullptr)
            {
               return std::move(newControl);
            }
         }

         return nullptr;
      }
   };
}
