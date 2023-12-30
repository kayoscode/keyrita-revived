#include <algorithm>

#include "XmlToUi.h"
#include "NuklearWindowRenderer.h"

using namespace pugi;

namespace wgui
{
   std::vector<std::unique_ptr<GuiControlFactoryBase>> XmlToUiUtil::mControlFactories;
   AttributeParser XmlToUiUtil::mAttributeParser;

   void SetAttributes(const xml_node_iterator& ctrl, GuiControlBase* pCtrl, AttributeParser& attributeParser)
   {
      for (xml_attribute_iterator attr = ctrl->attributes_begin(); attr != ctrl->attributes_end(); ++attr)
      {
         bool attrExists = pCtrl->AttributeExists(attr->name());
         attr_type_id_t typeHint = AttributeTypeManager::NotAType;

         if (attrExists)
         {
            // Try to parse it using the hinted attribute type.
            typeHint = pCtrl->GetAttributeType(attr->name());
         }

         std::unique_ptr<Attribute> parsedAttr = nullptr;

         // If we aren't expecting it to be a string, make it an int.
         if (typeHint != AttrTypes::Str())
         {
             parsedAttr = attributeParser.ParseAttribute(attr->value(), typeHint);
         }

         // Default to string if we couldn't parse it.
         if (parsedAttr == nullptr)
         {
            parsedAttr = std::make_unique<Attribute>();
            parsedAttr->SetType<AttrString>();
            parsedAttr->As<AttrString>()->Set(attr->value());
         }

         // Hopefully we parsed the attribute into the hinted type. If we didn't throw a warning here.
         // Set the attribute to this value reguardless. If the program crashes later on, the warning should indicate why.
         if (parsedAttr->GetType() != typeHint)
         {
            if (attrExists)
            {
               Application::Logger.error("Overwriting attribute {str} - incorrect type {str}!={str} value='{str}'",
                  attr->name(),
                  attributeParser.GetTypeManager().GetTypeName(parsedAttr->GetType()).c_str(),
                  attributeParser.GetTypeManager().GetTypeName(typeHint).c_str(),
                  attr->value());
            }
            else
            {
               pCtrl->GetAttributes()->Set(attr->name(), std::move(parsedAttr));
            }
         }
         else
         {
            pCtrl->GetAttributes()->Get(attr->name())->SetValue(std::move(parsedAttr));
         }
      }
   }

   void XmlToUiUtil::ConstructControls
      (const xml_object_range<xml_node_iterator>& doc, GuiControlBase* parent,
         std::vector<std::unique_ptr<GuiControlBase>>& ownedControls,
         std::vector<GuiControlBase*>& resultControls)
   {
      if (parent != nullptr)
      {
         if (!parent->SupportsChildren() && !doc.empty())
         {
            Application::Logger.error("Control of type: '{str}' does not support child controls",
               parent->GetLabel());
            return;
         }
      }

      for (xml_node_iterator ctrl = doc.begin(); ctrl != doc.end(); ++ctrl)
      {
         // Create the control.
         std::unique_ptr<GuiControlBase> control = CreateControl(ctrl->name());

         if (control != nullptr)
         {
            GuiControlBase* pCtrl = control.get();
            ownedControls.push_back(std::move(control));

            if (parent != nullptr)
            {
               parent->AddChild(pCtrl);
            }
            else
            {
               resultControls.push_back(pCtrl);
            }

            ConstructControls(ctrl->children(), pCtrl, ownedControls, resultControls);
            SetAttributes(ctrl, pCtrl, mAttributeParser);
         }
         else
         {
            Application::Logger.error("No factory definition to build a control of type '{str}'",
               ctrl->name());
            assert(false);
         }
      }
   }

   bool XmlToUiUtil::ConstructLayoutFromXmlFile(const std::string& fileName, 
      std::vector<std::unique_ptr<GuiControlBase>>& ownedControls,
      std::vector<GuiControlBase*>& controlTree)
   {
      ownedControls.clear();
      xml_document doc;
      xml_parse_result parseResult = doc.load_file(fileName.c_str());

      if (parseResult == -1)
      {
         return false;
      }

      if (doc.empty() || std::string(doc.first_child().name()) != "GuiRoot")
      {
         Application::Logger.error("Expected a root of 'GuiRoot' -> will not parse");
         return false;
      }

      if (doc.empty())
      {
         return false;
      }

      // Here, we will only work within the window tags. Ignore everything else.
      ConstructControls(doc.first_child().children(), nullptr, ownedControls, controlTree);

      return true;
   }

   bool XmlRenderer::ConstructLayoutFromXmlFile(const std::string& fileName)
   {
      return XmlToUiUtil::ConstructLayoutFromXmlFile(fileName, mOwnedControls, mControls);
   }
}