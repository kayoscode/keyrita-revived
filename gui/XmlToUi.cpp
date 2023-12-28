#include <algorithm>

#include "NuklearWindowRenderer.h"

using namespace pugi;

namespace wgui
{
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
                  attributeParser.GetTypeManager().GetTypeName(parsedAttr->GetType()),
                  attributeParser.GetTypeManager().GetTypeName(typeHint),
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

   void XmlLoadedWindowRenderer::ConstructComponents
      (const xml_object_range<xml_node_iterator>& doc, GuiControlBase* parent)
   {
      if (!parent->SupportsChildren() && !doc.empty())
      {
         Application::Logger.error("Control of type: '{str}' does not support child controls",
            parent->GetLabel());
         return;
      }

      for (xml_node_iterator ctrl = doc.begin(); ctrl != doc.end(); ++ctrl)
      {
         // Create the control.
         std::unique_ptr<GuiControlBase> control = CreateControl(ctrl->name());

         if (control != nullptr)
         {
            GuiControlBase* pCtrl = control.get();
            mOwnedControls.push_back(std::move(control));
            parent->AddChild(pCtrl);

            ConstructComponents(ctrl->children(), pCtrl);
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

   void XmlLoadedWindowRenderer::ConstructWindows(const xml_object_range<xml_node_iterator>& doc)
   {
      for (xml_node_iterator ctrl = doc.begin(); ctrl != doc.end(); ++ctrl)
      {
         // Create the control.
         std::unique_ptr<GuiControlBase> control = CreateControl(ctrl->name());

         if (control != nullptr)
         {
            GuiControlBase* pWindow = control.get();
            mOwnedControls.push_back(std::move(control));

            WindowRendererGui::AddChild(pWindow);
            SetAttributes(ctrl, pWindow, mAttributeParser);
            ConstructComponents(ctrl->children(), pWindow);
         }
         else
         {
            Application::Logger.error("No factory definition to build a control of type '{str}'",
               ctrl->name());
            assert(false);
         }
      }
   }

   bool XmlLoadedWindowRenderer::ConstructLayoutFromXml(const std::string& xmlString)
   {
      mOwnedControls.clear();
      xml_document doc;
      xml_parse_result parseResult = doc.load_string(xmlString.c_str());

      if (parseResult == -1)
      {
         return false;
      }

      if (doc.empty() || doc.first_child().name() != "GuiRoot")
      {
         Application::Logger.error("Expected a root of 'GuiRoot' -> will not parse");
         return;
      }

      if (doc.empty())
      {
         return false;
      }

      ConstructWindows(doc.first_child().children());
      return true;
   }

   bool XmlLoadedWindowRenderer::ConstructLayoutFromXmlFile(const std::string& fileName)
   {
      mOwnedControls.clear();
      xml_document doc;
      xml_parse_result parseResult = doc.load_file(fileName.c_str());

      if (parseResult == -1)
      {
         return false;
      }

      if (doc.empty() || std::string(doc.first_child().name()) != "GuiRoot")
      {
         Application::Logger.error("Expected a root of 'GuiRoot' -> will not parse");
         assert(false);
      }

      if (doc.empty())
      {
         return false;
      }

      // Here, we will only work within the window tags. Ignore everything else.
      ConstructWindows(doc.first_child().children());

      return true;
   }
}