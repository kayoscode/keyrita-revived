#include "Attributes.h"

namespace wgui
{
   attr_type_id_t AttrTypes::IntId = AttributeTypeManager::NotAType;
   attr_type_id_t AttrTypes::RealId = AttributeTypeManager::NotAType;
   attr_type_id_t AttrTypes::BoolId = AttributeTypeManager::NotAType;
   attr_type_id_t AttrTypes::StrId = AttributeTypeManager::NotAType;

   void AttrTypes::InitTypes(AttributeTypeManager& typeManager)
   {
      IntId = typeManager.AddType((std::string)IntTypeName, 1);
      RealId = typeManager.AddType((std::string)RealTypeName, 0);
      BoolId = typeManager.AddType((std::string)BoolTypeName, 0);
      StrId = typeManager.AddType((std::string)StrTypeName, 0);
   }
}
