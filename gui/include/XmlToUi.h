#pragma once

#include <vector>
#include <map>
#include <memory>

#include "Attributes.h"
#include "pugixml.hpp"
#include "Window.h"
#include "Attributes.h"
#include "NuklearWindowRenderer.h"

namespace wgui
{
   /// <summary>
   /// Utility class for maintaining the state of the control factories
   /// and for parsing xml to a control tree.
   /// </summary>
   class XmlToUiUtil
   {
   private:
      class StandardControlFactory : public GuiControlFactoryBase
      {
      public:
         void Init() override;
      };

      static std::vector<std::unique_ptr<GuiControlFactoryBase>> mControlFactories;
      static AttributeParser mAttributeParser;

   public:

      static void Init()
      {
         AddControlFactory<StandardControlFactory>();
      }

      /// <summary>
      /// Uses the XML to parse a list of subnodes.
      /// Everything is listed under the GuiRoot tag.
      /// </summary>
      /// <param name="fileName"></param>
      /// <param name="ownedControls"></param>
      /// <returns></returns>
      static bool ConstructLayoutFromXmlFile(const std::string& fileName, 
         std::vector<std::unique_ptr<GuiControlBase>>& ownedControls,
         std::vector<GuiControlBase*>& controlTree);

      template <class T>
      static void AddControlFactory()
         requires std::is_base_of_v<GuiControlFactoryBase, T>
      {
         Application::Logger.trace("Initializing new control factory");

         mControlFactories.push_back(std::make_unique<T>());
         mControlFactories[mControlFactories.size() - 1]->Init();

         std::vector<std::string> newControls = mControlFactories[mControlFactories.size() - 1]->ListRegisteredControls();
         std::string ctrls;
         for (int i = 0; i < newControls.size(); i++)
         {
            ctrls += newControls[i] + ((i < newControls.size() - 1) ? ", " : " ");
         }

         Application::Logger.trace("Added controls {str}", ctrls.c_str());
      }

   private:
      static void ConstructControls(const pugi::xml_object_range<pugi::xml_node_iterator>& document,
         GuiControlBase* parent,
         std::vector<std::unique_ptr<GuiControlBase>>& ownedControls,
         std::vector<GuiControlBase*>& resultControls);

      static std::unique_ptr<GuiControlBase> CreateControl(const std::string& controlName)
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