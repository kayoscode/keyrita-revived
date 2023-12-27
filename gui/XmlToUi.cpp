#include <algorithm>

#include "NuklearWindowRenderer.h"

using namespace pugi;

namespace wgui
{
   bool ParseToInt(const std::string& value, int64_t& intVal)
   {
      size_t idx = 0;
      try 
      {
         intVal = std::stoll(value, &idx, 10);
      }
      catch (std::exception)
      {
         return false;
      }

      // Make sure the value in the quotes was an integer.
      if (idx != value.size())
      {
         intVal = 0ll;
         // TODO: warning here
         return false;
      }

      return true;
   }

   bool ParseToReal(const std::string& value, double& dVal)
   {
      size_t idx = 0;

      try
      {
         dVal = std::stod(value, &idx);
      }
      catch (std::exception)
      {
         return false;
      }

      // Make sure the value in the quotes was an integer.
      if (idx != value.size())
      {
         dVal = 0.0;
         // TODO: warning here
         return false;
      }

      return true;
   }

   bool ParseToPercent(const std::string& value, double& dVal)
   {
      if (value[value.size() - 1] == '%')
      {
         if (ParseToReal(value.substr(0, value.size() - 1), dVal))
         {
            dVal /= 100;
            return true;
         }
      }

      return false;
   }

   bool ParseToBool(const std::string& value, bool& bValue)
   {
      size_t idx = 0;
      std::string cpy(value);
      std::transform(cpy.begin(), cpy.end(), cpy.begin(), ::toupper);

      if (cpy == "TRUE")
      {
         bValue = true;
         return true;
      }
      else if (cpy == "FALSE")
      {
         bValue = false;
         return true;
      }

      return false;
   }

   void SetAttributes(const xml_node_iterator& ctrl, GuiControlBase* pCtrl)
   {
      for (xml_attribute_iterator attr = ctrl->attributes_begin(); attr != ctrl->attributes_end(); ++attr)
      {
         bool attrExists = pCtrl->AttributeExists(attr->name());
         eAttributeType parsedType = eAttributeType::Int;
         eAttributeType type = parsedType;

         if (attrExists)
         {
            type = pCtrl->GetAttributeType(attr->name());
         }

         // Figure out the best type to parse it from.
         int64_t iValue;
         double dValue;
         bool bValue;

         if (ParseToBool(attr->value(), bValue))
         {
            pCtrl->SetAttributeBool(attr->name(), bValue);
            eAttributeType parsedType = eAttributeType::Bool;
         }
         else if (ParseToInt(attr->value(), iValue))
         {
            pCtrl->SetAttributeInt(attr->name(), iValue);
            eAttributeType parsedType = eAttributeType::Int;
         }
         else if (ParseToReal(attr->value(), dValue) ||
                  ParseToPercent(attr->value(), dValue))
         {
            pCtrl->SetAttributeReal(attr->name(), dValue);
            eAttributeType parsedType = eAttributeType::Real;
         }
         else 
         {
            pCtrl->SetAttributeString(attr->name(), attr->value());
            eAttributeType parsedType = eAttributeType::String;
         }

         if (attrExists && type != parsedType)
         {
            // TODO: add warning, the type written to the file doesn't match the expected type.
         }
      }
   }

   void XmlLoadedWindowRenderer::ConstructComponents
      (const xml_object_range<xml_node_iterator>& doc, GuiControlBase* parent)
   {
      if (!parent->SupportsChildren() && !doc.empty())
      {
         // Add error here.
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
            SetAttributes(ctrl, pCtrl);

            ConstructComponents(ctrl->children(), pCtrl);
         }
         else
         {
            // TODO: error case.
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
            SetAttributes(ctrl, pWindow);
            ConstructComponents(ctrl->children(), pWindow);
         }
         else
         {
            // TODO: error case.
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
         // TODO: throw error telling user they should use GuiRoot;
         assert(false);
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
         // TODO: throw error telling user they should use GuiRoot;
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