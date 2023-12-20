#pragma once

#include <vector>
#include <string>

struct nk_context;

namespace wgui
{
   class WindowBase;

   enum eControlType
   {
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
      /// <summary>
      /// Renders the control to the screen.
      /// </summary>
      /// <param name="window"></param>
      /// <param name="context"></param>
      virtual void Render(WindowBase* const window, nk_context* context) = 0;
      virtual eControlType GetControlType() const = 0;
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
      GuiLabel(const std::string& text, eTextAlignmentFlags alignment)
         : mText(text), mTextAlignment(alignment)
      {
      }

      void Render(WindowBase* const window, nk_context* context) override;

      void SetTextAlignment(eTextAlignmentFlags textAlign) { mTextAlignment = textAlign; }
      void SetText(const std::string& text) { mText = text; }
   private:
      eTextAlignmentFlags mTextAlignment;
      std::string mText;
   };

#pragma endregion


#pragma region Row Layouts

   class GuiLayoutRowBase : public GuiControlBase
   {
   public:
      GuiLayoutRowBase(int height, int minCols = 0) 
         : mHeight(height), mMinCols(minCols) { }

      eControlType GetControlType() const override 
      { 
         return eControlType::LayoutRow;
      }

      void AddControl(GuiControlBase* newControl)
      {
         // Make sure it's not another layout row. These cannot be nested.
         //_ASSERT((newControl->GetControlType() != eControlType::LayoutRow) &&
                 //(newControl->GetControlType() != eControlType::Menu)
         //);

         mControls.push_back(newControl);
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
      int mHeight;
      int mMinCols;
   };

   /// <summary>
   /// A grid of evenly spaced columns.
   /// </summary>
   class GuiLayoutRowDynamic : public GuiLayoutRowBase
   {
   public:
      GuiLayoutRowDynamic(int height, int minCols = 0) : GuiLayoutRowBase(height, minCols) { }
      void Render(WindowBase* const window, nk_context* context) override;
   };

   /// <summary>
   /// A grid of fixed sized columns
   /// </summary>
   class GuiLayoutRowStatic : public GuiLayoutRowBase
   {
   public:
      GuiLayoutRowStatic(int height, int colWidth, int minCols = 0) 
         : mColWidth(colWidth),
           GuiLayoutRowBase(height, minCols) { }

      void Render(WindowBase* const window, nk_context* context) override;

   private:
      int mColWidth;
   };

   /// <summary>
   /// A grid of widgets which can have percentage scaled widths.
   /// </summary>
   class GuiLayoutRowDynamicGrid : public GuiLayoutRowBase
   {
   public:
      GuiLayoutRowDynamicGrid(int height) : GuiLayoutRowBase(height) { }
   };

   /// <summary>
   /// A grid of widgets which can have constant sized widths.
   /// </summary>
   class GuiLayoutRowStaticGrid : public GuiLayoutRowBase
   {
   public:
      GuiLayoutRowStaticGrid(int height) : GuiLayoutRowBase(height) { }
   };

   /// <summary>
   /// A combination of statically and dynamically spaced columns.
   /// </summary>
   class GuiLayoutRowMixedGrid : public GuiLayoutRowBase
   {
   public:
      GuiLayoutRowMixedGrid(int height) : GuiLayoutRowBase(height) { }
   };

   /// <summary>
   /// Allows you to specify the height and width of columns in the grid (up to the max height)
   /// Statically spaced columns
   /// </summary>
   class GuiLayoutStaticSpaceGrid : public GuiLayoutRowBase
   {
   public:
      GuiLayoutStaticSpaceGrid(int height) : GuiLayoutRowBase(height) { }
   };

   /// <summary>
   /// Allows you to specify the height and width of columns in the grid (up to the max height)
   /// Dynamically spaced columns
   /// </summary>
   class GuiLayoutDynamicSpaceGrid : public GuiLayoutRowBase
   {
   public:
      GuiLayoutDynamicSpaceGrid(int height) : GuiLayoutRowBase(height) { }
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
   class GuiWindow
   {
   public:
      GuiWindow(const std::string& name,
         int posX, int posY, int width, int height,
         const std::string& title = "")
         : mName(name),
         mPosX(posX), mPosY(posY),
         mWidth(width), mHeight(height),
         mTitle(title.empty() ? name : title)
      {
      }

      void AddControl(GuiControlBase* control)
      {
         mControls.push_back(control);
      }

      virtual void Render(WindowBase* const window, nk_context* context);

      inline void SetPos(int x, int y)
      {
         mPosX = x;
         mPosY = y;
      }

      inline void GetPos(int& x, int& y)
      {
         x = mPosX;
         y = mPosY;
      }

      inline void SetDims(int width, int height)
      {
         mWidth = width;
         mHeight = height;
      }

      inline void GetDims(int& width, int& height)
      {
         width = mWidth;
         height = mHeight;
      }

      inline void SetTitle(const std::string& title) { mTitle = title; }

   private:
      int mPosX, mPosY;
      int mWidth, mHeight;
      std::string mName, mTitle;
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

   /// <summary>
   /// Renders the gui based on the controls and layouts given by the developer.
   /// </summary>
   class WindowRendererGui : public WindowRenderer
   {
   public:
      void RenderStart(WindowBase* const window, nk_context* context) override;
      void Render(WindowBase* const window, nk_context* context) override;
      void RenderFinish(WindowBase* const window, nk_context* delta) override;

      void AddWindow(GuiWindow* window)
      {
         mWindows.push_back(window);
      }

   private:
      std::vector<GuiWindow*> mWindows;
   };
}
