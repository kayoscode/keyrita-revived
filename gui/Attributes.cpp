#include "Attributes.h"
#include "StringUtil.h"

namespace
{
   static bool ParseToInt(const std::string& value, int64_t& intVal)
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
         return false;
      }

      return true;
   }

   static bool ParseToReal(const std::string& value, double& dVal)
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
         return false;
      }

      return true;
   }

   static bool ParseToPercent(const std::string& value, double& dVal)
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

   static bool ParseToBool(const std::string& value, bool& bValue)
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

   /// <summary>
   /// Flags have the following syntax: 
   /// Flags:COMMA_SEPARATED_LIST_OF_NAMED_FLAGS
   /// </summary>
   /// <param name="value"></param>
   /// <param name="flagsValue"></param>
   /// <param name="flagParseFunc"></param>
   /// <returns></returns>
   static bool ParseToFlags(const std::string& value, int& flagsValue,
      std::function<int(std::string& text)> flagParseFunc)
   {
      flagsValue = 0;
      
      std::string cpy(value);
      std::transform(cpy.begin(), cpy.end(), cpy.begin(), ::toupper);

      int colIdx = cpy.find(':');
      if (colIdx < 0)
      {
         return false;
      }

      if (cpy.substr(0, colIdx) == "FLAGS")
      {
         int flagIdx = colIdx + 1;
         std::vector<std::string> allFlags;
         StringUtil::SplitString(cpy.substr(flagIdx), ',', allFlags);

         for (int i = 0; i < allFlags.size(); i++)
         {
            int flag = flagParseFunc(allFlags[i]);

            if (flag == -1)
            {
               return false;
            }

            flagsValue |= flag;
         }

         return true;
      }

      return false;
   }
}

namespace wgui
{
   attr_type_id_t AttrTypes::IntId = AttributeTypeManager::NotAType;
   attr_type_id_t AttrTypes::RealId = AttributeTypeManager::NotAType;
   attr_type_id_t AttrTypes::BoolId = AttributeTypeManager::NotAType;
   attr_type_id_t AttrTypes::StrId = AttributeTypeManager::NotAType;
   attr_type_id_t AttrTypes::WinFlagsId = AttributeTypeManager::NotAType;
   attr_type_id_t AttrTypes::AlignFlagsId = AttributeTypeManager::NotAType;

   void AttrTypes::InitTypes(AttributeTypeManager& typeManager)
   {
      IntId = typeManager.AddType((std::string)IntTypeName, 1);
      RealId = typeManager.AddType((std::string)RealTypeName, 0);
      BoolId = typeManager.AddType((std::string)BoolTypeName, 0);
      StrId = typeManager.AddType((std::string)StrTypeName, 0);
      WinFlagsId = typeManager.AddType((std::string)WindowFlagsName, 0);
      AlignFlagsId = typeManager.AddType((std::string)TextAlignFlagsName, 0);
   }

   void StandardAttributeTypesParseFactory::Init(AttributeTypeManager& typeManager)
   {
      AttrTypes::InitTypes(typeManager);
      mAttributeCreationFuncs.emplace(std::make_pair(AttrTypes::Int(),
         [](const std::string& text) -> std::unique_ptr<Attribute>
         {
            int64_t value;

            if (ParseToInt(text, value))
            {
               auto attr = std::make_unique<Attribute>();
               attr->SetType<AttrInt>();
               attr->As<AttrInt>()->Set(value);
               return std::move(attr);
            }

            return nullptr;
         }));

      mAttributeCreationFuncs.emplace(std::make_pair(AttrTypes::Real(),
         [](const std::string& text) -> std::unique_ptr<Attribute>
         {
            double dVal;

            if (ParseToReal(text, dVal) || ParseToPercent(text, dVal))
            {
               auto attr = std::make_unique<Attribute>();
               attr->SetType<AttrReal>();
               attr->As<AttrReal>()->Set(dVal);
               return std::move(attr);
            }

            return nullptr;
         }));

      mAttributeCreationFuncs.emplace(std::make_pair(AttrTypes::Bool(),
         [](const std::string& text) -> std::unique_ptr<Attribute>
         {
            bool bVal;

            if (ParseToBool(text, bVal))
            {
               auto attr = std::make_unique<Attribute>();
               attr->SetType<AttrBool>();
               attr->As<AttrBool>()->Set(bVal);
               return std::move(attr);
            }

            return nullptr;
         }));

      mAttributeCreationFuncs.emplace(std::make_pair(AttrTypes::WinFlags(),
         [](const std::string& text) -> std::unique_ptr<Attribute>
         {
            int flagsValue;

            if (ParseToFlags(text, flagsValue, WindowFlagsUtil::GetFlagFor))
            {
               auto attr = std::make_unique<Attribute>();
               attr->SetType<AttrWinFlags>();
               attr->As<AttrWinFlags>()->Set(flagsValue);
               return std::move(attr);
            }

            return nullptr;
         }));

      mAttributeCreationFuncs.emplace(std::make_pair(AttrTypes::AlignFlags(),
         [](const std::string& text) -> std::unique_ptr<Attribute>
         {
            int flagsValue;

            if (ParseToFlags(text, flagsValue, TextAlignFlagsUtil::GetFlagFor))
            {
               auto attr = std::make_unique<Attribute>();
               attr->SetType<AttrAlignFlags>();
               attr->As<AttrAlignFlags>()->Set(flagsValue);
               return std::move(attr);
            }

            return nullptr;
         }));
   }
}
