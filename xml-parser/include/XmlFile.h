#pragma once

#include <string>
#include <vector>
#include <exception>
#include <iostream>
#include <memory>
#include <map>
#include <concepts>
#include <stdint.h>

namespace xml
{
   enum class eXmlType
   {
      Int,
      Real,
      String
   };

#pragma region Attributes

   class XmlValue
   {
   public:
      XmlValue(eXmlType type)
         : mType(type) { }

   private:
      eXmlType mType;
   };

   class XmlInt : public XmlValue
   {
   public:
      XmlInt()
         : XmlValue(eXmlType::Int) {}

      int64_t Get() const { return mValue; }
      operator int64_t() const { return mValue; }

      XmlInt Set(int64_t value)
      {
         mValue = value;
         return *this;
      }

      XmlInt operator = (int64_t value)
      {
         mValue = value;
         return *this;
      }

   private:
      int64_t mValue;
   };

   class XmlReal : public XmlValue
   {
   public:
      XmlReal()
         : XmlValue(eXmlType::Real) {}

      double Get() const { return mValue; }
      operator double() const { return mValue; }

      XmlReal Set(double value)
      {
         mValue = value;
         return *this;
      }

      XmlReal operator = (double value)
      {
         mValue = value;
         return *this;
      }

   private:
      double mValue;
   };
   
   class XmlString : public XmlValue
   {
   public:
      XmlString()
         : XmlValue(eXmlType::String) {}

      std::string Get() const { return mValue; }
      operator std::string() const { return mValue; }

      XmlString Set(const std::string& value)
      {
         mValue = value;
         return *this;
      }

      XmlString operator = (const std::string& value)
      {
         mValue = value;
         return *this;
      }

   private:
      std::string mValue;
   };

   class XmlAttribute
   {
   public:
      XmlAttribute() { }
      XmlAttribute(eXmlType type)
      {
         SetType(type);
      }

      template<class T>
      T* As()
      {
         if (!mValue)
         {
            throw std::exception("Attribute not initialized to a value");
         }

         return reinterpret_cast<T*>(mValue.get());
      }

      void SetType(eXmlType type)
      {
         switch (type)
         {
         case eXmlType::Int:
            mValue = std::make_unique<XmlInt>();
            break;
         case eXmlType::Real:
            mValue = std::make_unique<XmlReal>();
            break;
         case eXmlType::String:
            mValue = std::make_unique<XmlString>();
            break;
         }
      }

   private:
      std::unique_ptr<XmlValue> mValue;
   };

   class AttributeSet
   {
   public:
      XmlAttribute* operator[](const std::string& name) const
      {
         auto attr = mAttributes.find(name);

         if (attr == mAttributes.end())
         {
            throw std::out_of_range("Attribute with the given name was not found.");
         }

         return attr->second.get();
      }

      XmlAttribute* Add(const std::string& name)
      {
         auto attr = mAttributes.find(name);

         if (attr != mAttributes.end())
         {
            throw std::out_of_range("Attribute with given name already exists.");
         }

         mAttributes.emplace(name, std::make_unique<XmlAttribute>());
         return mAttributes[name].get();
      }

   private:
      std::map<std::string, std::unique_ptr<XmlAttribute>> mAttributes;
   };

#pragma endregion

#pragma region Elements

   class XmlElement
   {
   public:
      XmlElement(const std::string& tag)
         : mTagName(tag)
      {
      }

      XmlElement* operator[](int index) const
      {
         if (index > mElements.size())
         {
            throw std::out_of_range("Index out of range of element set.");
         }

         return mElements[index].get();
      }

      XmlElement* GetIndex(int index) const
      {
         return (*this)[index];
      }

      /// <summary>
      /// Returns a list of all elements holding a given tag.
      /// </summary>
      /// <param name="key"></param>
      /// <returns></returns>
      std::vector<XmlElement*> operator[](const std::string& key) const
      {
         for (const auto& elem : mElements)
         {

         }
      }

      /// <summary>
      /// Finds and returns the first element with the given tag. Null if the tag doesn't exist.
      /// </summary>
      /// <param name="tag"></param>
      /// <returns></returns>
      XmlElement* FindFirst(const std::string& tag) const
      {
         for (const auto& elem : mElements)
         {
            if (elem->Tag() == tag)
            {
               return elem.get();
            }
         }

         return nullptr;
      }

      std::vector<XmlElement*> FindAll(const std::string& tag) const
      {
         return (*this)[tag];
      }

      void SetTag(const std::string& tag)
      {
         mTagName = tag;
      }

      /// <summary>
      /// Adds an owning copy of an element.
      /// </summary>
      /// <param name="element"></param>
      void AddElement(std::unique_ptr<XmlElement> element)
      {
         mElements.push_back(std::move(element));
      }

      AttributeSet* Attributes() { return mAttributes.get(); }

      std::string Tag() const { return mTagName; }

   private:
      std::vector<std::unique_ptr<XmlElement>> mElements;
      std::string mTagName;
      std::unique_ptr<AttributeSet> mAttributes;
   };

#pragma endregion

   class XmlSerializer
   {
   public:
      /// <summary>
      /// Loads the data rom the xml text and returns an element representing the head node.
      /// </summary>
      /// <param name="xmlText"></param>
      /// <returns></returns>
      static std::unique_ptr<XmlElement> LoadXml(const std::ifstream& xmlText);

      enum class eAttributeOrder 
      { 
         None, 
         Alphabetical, 
      };

      /// <summary>
      /// Properties describing the beautification options for serializing to Xml.
      /// </summary>
      struct tXmlBeautifyConfig
      {
         /// <summary>
         /// Indicates whether there should be a new line at the start and end of each element.
         /// </summary>
         bool NewLines = true;

         /// <summary>
         /// Describes how the beautifier will produce tabs.
         /// With tabs off, none of the other properties will do anything.
         /// Tabs as spaces being set to true will replace tabs in the beautifier with spaces.
         /// TabSpaceWidth sets how many spaces to use.
         /// </summary>
         bool Tabs = true;
         bool TabsAsSpaces = true;
         int TabSpaceWidth = 3;

         /// <summary>
         /// Specifies the number of tabs the serialized xml starts with.
         /// </summary>
         int InitialIndex = 0;

         /// <summary>
         /// If verbose is set to true, tags that have no children 
         /// will still contain an open and close section of the tag.
         /// <no_child /> => <no_child></no_child>
         /// </summary>
         bool Verobse = false;

         /// <summary>
         /// Specifies whether elements should have a special order.
         /// </summary>
         eAttributeOrder AttributeOrdering = eAttributeOrder::None;

         static tXmlBeautifyConfig Default()
         {
            return tXmlBeautifyConfig();
         }
      };

      /// <summary>
      /// Saves an xml data set to a string.
      /// </summary>
      /// <param name="head"></param>
      /// <param name="outputText"></param>
      static void SerializeXml(XmlElement* head, std::string& outputText, const tXmlBeautifyConfig& config);
   };
}