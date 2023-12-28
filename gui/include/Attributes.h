#pragma once

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <exception>
#include <cassert>
#include <functional>
#include <cstdint>
#include <memory>

namespace wgui
{
   typedef int64_t attr_type_id_t;

   struct CaseInsensitiveStrCompare
   {
   public:
      bool operator()(const std::string& a, const std::string& b) const
      {
         return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(),
            [](char a, char b)
            {
               return std::tolower(a) < std::tolower(b);
            });
      }
   };

   class AttributeTypeManager
   {
   public:
      static constexpr attr_type_id_t NotAType = -1;
      static constexpr uint32_t DefaultTypePriority = 0;

      /// <summary>
      /// Adds a type to the system and gives it a priority value.
      /// Higher priority means that type will be preferred if there's a conflict between two types when parsing.
      /// Equal priority will prioritize the first one it finds.
      /// </summary>
      /// <param name="typeName"></param>
      /// <param name="typePriority"></param>
      /// <returns></returns>
      attr_type_id_t AddType(const std::string& typeName, uint32_t typePriority = DefaultTypePriority)
      {
         const auto& attr = mAttributeTypes.find(typeName);

         if (attr == mAttributeTypes.end())
         {
            mNextType++;
            mAttributeTypes[typeName] = mNextType;
            mAttributeTypeNames[mNextType] = typeName;
            mTypePriorities.push_back(typePriority);
            return mNextType;
         }

         return NotAType;
      }

      attr_type_id_t GetType(const std::string& typeName) const
      {
         const auto& attr = mAttributeTypes.find(typeName);

         if (attr != mAttributeTypes.end())
         {
            return attr->second;
         }

         return NotAType;
      }

      uint32_t GetTypePriority(attr_type_id_t typeId) const
      {
         if (typeId >= 0 && typeId < mTypePriorities.size())
         {
            return mTypePriorities[typeId];
         }

         return -1;
      }

      std::string GetTypeName(attr_type_id_t id)
      {
         if (mAttributeTypeNames.find(id) != mAttributeTypeNames.end())
         {
            return mAttributeTypeNames[id];
         }

         assert(false);
         return "Invalid type: " + std::to_string(id);
      }

   private:
      std::map<std::string, attr_type_id_t, CaseInsensitiveStrCompare> mAttributeTypes;
      std::map<attr_type_id_t, std::string> mAttributeTypeNames;
      std::vector<uint32_t> mTypePriorities;

      attr_type_id_t mNextType = NotAType;
   };

   class AttrTypes
   {
   public:
      static void InitTypes(AttributeTypeManager& typeManager);

      static constexpr std::string_view IntTypeName = "Int";
      static constexpr std::string_view RealTypeName = "Real";
      static constexpr std::string_view BoolTypeName = "Bool";
      static constexpr std::string_view StrTypeName = "Str";

      inline static attr_type_id_t Int() { return IntId; }
      inline static attr_type_id_t Real() { return RealId; }
      inline static attr_type_id_t Bool() { return BoolId; }
      inline static attr_type_id_t Str() { return StrId; }

   private:
      static attr_type_id_t IntId;
      static attr_type_id_t RealId;
      static attr_type_id_t BoolId;
      static attr_type_id_t StrId;
   };

   class CtrlAttribute
   {
   public:
      CtrlAttribute(attr_type_id_t typeId)
         : mType(typeId) { }

      attr_type_id_t GetType() const { return mType; }

      /// <summary>
      /// Must override this. If copy is called, you should assume
      /// the underlying data of the incoming attribute is the same as this type.
      /// </summary>
      /// <param name="other"></param>
      virtual void Copy(CtrlAttribute* other) = 0;

   private:
      attr_type_id_t mType;
   };

   class AttrInt : public CtrlAttribute
   {
   public:
      AttrInt()
         : mValue(0), CtrlAttribute(AttrTypes::Int()) {}

      int64_t Get() const { return mValue; }
      operator int64_t() const { return mValue; }

      void Copy(CtrlAttribute* other) override
      {
         mValue = reinterpret_cast<AttrInt*>(other)->mValue;
      }

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
         : mValue(0.0), CtrlAttribute(AttrTypes::Real()) {}

      double Get() const { return mValue; }
      operator double() const { return mValue; }

      void Copy(CtrlAttribute* other) override
      {
         mValue = reinterpret_cast<AttrReal*>(other)->mValue;
      }

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
         : CtrlAttribute(AttrTypes::Str()) {}

      std::string Get() const { return mValue; }
      operator std::string() const { return mValue; }

      void Copy(CtrlAttribute* other) override
      {
         mValue = reinterpret_cast<AttrString*>(other)->mValue;
      }

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

   class AttrBool : public CtrlAttribute
   {
   public:
      AttrBool()
         : mValue(false), CtrlAttribute(AttrTypes::Bool())
      {
      }

      bool Get() const { return mValue; }
      operator bool() const { return mValue; }

      void Copy(CtrlAttribute* other) override
      {
         mValue = reinterpret_cast<AttrBool*>(other)->mValue;
      }

      AttrBool Set(bool value)
      {
         mValue = value;
         return *this;
      }

      AttrBool operator = (bool value)
      {
         mValue = value;
         return *this;
      }

      bool& GetRef() { return mValue; }

   private:
      bool mValue;
   };

   class Attribute
   {
   public:
      Attribute()
         : mValue(nullptr)
      {
      }

      attr_type_id_t GetType() 
      { 
         if (mValue != nullptr)
         {
            return mValue->GetType();
         }

         return AttributeTypeManager::NotAType;
      }

      void SetValue(std::unique_ptr<Attribute> other)
      {
         if (mValue == nullptr)
         {
            mValue = std::move(other->mValue);
         }
         else
         {
            mValue->Copy(other->mValue.get());
         }
      }

      template<class T>
      T* As()
         requires std::is_base_of_v<CtrlAttribute, T>
      {
         if (!mValue)
         {
            assert(false);
         }

         return reinterpret_cast<T*>(mValue.get());
      }

      template<class T>
      T* const As() const
         requires std::is_base_of_v<CtrlAttribute, T>
      {
         if (!mValue)
         {
            assert(false);
         }

         return reinterpret_cast<T* const>(mValue.get());
      }

      template<class T>
      bool Is() const
         requires std::is_base_of_v<CtrlAttribute, T>
      {
         if (dynamic_cast<T*>(mValue.get()) != nullptr)
         {
            return true;
         }

         return false;
      }

      template<typename T>
      void SetType()
         requires std::is_base_of_v<CtrlAttribute, T>
      {
         mValue = std::make_unique<T>();
      }

   private:
      std::unique_ptr<CtrlAttribute> mValue;
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

      template<typename T>
      Attribute* Get(const std::string& name) const
         requires std::is_base_of_v<CtrlAttribute, T>
      {
         return (*this)[name]->As<T>();
      }

      template<typename T>
      T* Add(const std::string& name)
         requires std::is_base_of_v<CtrlAttribute, T>
      {
         auto attr = mAttributes.find(name);

         if (attr != mAttributes.end())
         {
            throw std::out_of_range("Attribute with given name already exists.");
         }

         mAttributes.emplace(name, std::make_unique<Attribute>());
         mAttributes[name]->SetType<T>();
         return mAttributes[name].get()->As<T>();
      }

      /// <summary>
      /// Adds the attribute if it doens't exist and if it does sets it to the passed value.
      /// </summary>
      /// <param name="name"></param>
      /// <param name="value"></param>
      void Set(const std::string& name, std::unique_ptr<Attribute> value)
      {
         mAttributes[name] = std::move(value);
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

#pragma region Attribute Types

   class AttributeParseFactoryBase
   {
   public:
      typedef std::function<std::unique_ptr<Attribute>(const std::string& text)> CreateAttrFunc;

      /// <summary>
      /// Initialize the dictionary of attribute creation functions.
      /// </summary>
      /// <param name="typeManager"></param>
      virtual void Init(AttributeTypeManager& typeManager) = 0;

      /// <summary>
      /// Base implementation
      /// </summary>
      /// <param name="text"></param>
      /// <param name="typeHint"></param>
      /// <returns></returns>
      std::unique_ptr<Attribute> TryParseAttribute(const std::string& text, attr_type_id_t typeHint, 
         const AttributeTypeManager& typeManager)
      {
         std::unique_ptr<Attribute> currentAttr = nullptr;
         int lastFoundPriority = -1;

         for (auto itt = mAttributeCreationFuncs.begin(); itt != mAttributeCreationFuncs.end(); ++itt)
         {
            std::unique_ptr<Attribute> parsed = itt->second(text);

            if (parsed != nullptr)
            {
               if (itt->first == typeHint)
               {
                  return parsed;
               }

               if (typeManager.GetTypePriority(itt->first) > lastFoundPriority || currentAttr == nullptr)
               {
                  lastFoundPriority = typeManager.GetTypePriority(itt->first);
                  currentAttr = std::move(parsed);
               }
            }
         }

         return currentAttr;
      }

   protected:
      std::map<attr_type_id_t, CreateAttrFunc> mAttributeCreationFuncs;
   };

   class StandardAttributeTypesParseFactory : public AttributeParseFactoryBase
   {
   public:
      StandardAttributeTypesParseFactory() { }

      void Init(AttributeTypeManager& typeManager) override
      {
         AttrTypes::InitTypes(typeManager);

         mAttributeCreationFuncs.emplace(std::make_pair(AttrTypes::Int(), 
            [](const std::string& text) -> std::unique_ptr<Attribute>
            {
               size_t idx = 0;
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
               size_t idx = 0;
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
               size_t idx = 0;
               bool bVal;

               if (ParseToBool(text, bVal) || ParseToBool(text, bVal))
               {
                  auto attr = std::make_unique<Attribute>();
                  attr->SetType<AttrBool>();
                  attr->As<AttrBool>()->Set(bVal);
                  return std::move(attr);
               }

               return nullptr;
            }));
      }

   private:
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
   };

   class AttributeParser
   {
   public:
      AttributeParser()
      {
         AddAttributeParseFactory<StandardAttributeTypesParseFactory>();
      }

      template <class T>
      void AddAttributeParseFactory()
         requires std::is_base_of_v<AttributeParseFactoryBase, T>
      {
         mAttributeFactories.push_back(std::make_unique<T>());
         mAttributeFactories[mAttributeFactories.size() - 1]->Init(mTypeManager);
      }

      /// <summary>
      /// Parses an attribute. If the parse is successful, it will return an attribute of that type,
      /// otherwise it will return an attribute string. Type priority is ignored if the parsed type 
      /// is equal to the hinted type.
      /// </summary>
      /// <param name="text"></param>
      /// <param name="typeHint"></param>
      std::unique_ptr<Attribute> ParseAttribute(const std::string& text, attr_type_id_t typeHint = AttributeTypeManager::NotAType)
      {
         std::unique_ptr<Attribute> lastAttribute = nullptr;
         uint32_t lastParsedPriority = -1;

         for (int i = 0; i < mAttributeFactories.size(); i++)
         {
            std::unique_ptr<Attribute> parsedAttr = mAttributeFactories[i]->TryParseAttribute(text, typeHint, mTypeManager);

            if (parsedAttr != nullptr)
            {
               if (parsedAttr->GetType() == typeHint)
               {
                  return std::move(parsedAttr);
               }
               else if (mTypeManager.GetTypePriority(parsedAttr->GetType()) > lastParsedPriority || lastAttribute == nullptr)
               {
                  lastParsedPriority = mTypeManager.GetTypePriority(parsedAttr->GetType());
                  lastAttribute = std::move(parsedAttr);
               }
            }
         }

         return std::move(lastAttribute);
      }

      AttributeTypeManager& GetTypeManager() { return mTypeManager; }

   private:
      std::vector<std::unique_ptr<AttributeParseFactoryBase>> mAttributeFactories;
      AttributeTypeManager mTypeManager;
   };

#pragma endregion
}
