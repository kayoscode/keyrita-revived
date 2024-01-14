#pragma once

#include <concepts>
#include <iterator>
#include <map>
#include <memory>
#include <string>

#include "StandardControls.h"

template <typename T>
concept EqualityComparable = requires(T a, T b) {
   {
      a == b
   } -> std::same_as<bool>;
};

namespace wgui
{
typedef std::vector<std::unique_ptr<GuiControlBase>> owned_controls_t;

class OwnedControlsIterator
{
 public:
   using iterator_category = std::forward_iterator_tag;
   using value_type = GuiControlBase *;
   using difference_type = std::ptrdiff_t;
   using pointer = GuiControlBase **;
   using reference = GuiControlBase *&;

   OwnedControlsIterator(owned_controls_t::iterator node) : mCurrent(node)
   {
   }

   value_type operator*() const
   {
      return mCurrent->get();
   }
   value_type operator->() const
   {
      return mCurrent->get();
   }

   OwnedControlsIterator &operator++()
   {
      ++mCurrent;
      return *this;
   }

   bool operator==(const OwnedControlsIterator &other) const
   {
      return mCurrent == other.mCurrent;
   }

   bool operator!=(const OwnedControlsIterator &other) const
   {
      return !(*this == other);
   }

 private:
   owned_controls_t::iterator mCurrent;
};

/// <summary>
/// Utility methods to help access and manipulate owned controls.
/// Should be utilized when you have access to a vector of unique pointers to gui controls.
/// Renderer level, abstract control level.
/// </summary>
class ControlAccessUtils
{
 public:
   template <typename T, typename __ControlIterator = OwnedControlsIterator>
   static void GetControlsWithType(const __ControlIterator begin, const __ControlIterator end,
                                   std::vector<T *> &foundControls)
      requires std::is_base_of_v<GuiControlBase, T> && std::_Is_iterator_v<__ControlIterator>
   {
      for (auto it = begin; it != end; ++it)
      {
         T *val = dynamic_cast<T *>(*it);
         if (val != nullptr)
         {
            foundControls.push_back(val);
         }
      }
   }

   template <typename T = GuiControlBase, typename __ControlIterator = OwnedControlsIterator>
   static void GetControlsWithTag(const __ControlIterator begin, const __ControlIterator end, const std::string &tag,
                                  std::vector<T *> &foundControls)
      requires std::is_base_of_v<GuiControlBase, T> && std::_Is_iterator_v<__ControlIterator>
   {
      for (auto it = begin; it != end; ++it)
      {
         T *val = dynamic_cast<T *>(*it);
         if (val != nullptr && val->GetTag() == tag)
         {
            foundControls.push_back(val);
         }
      }
   }

   template <typename T = GuiControlBase, typename __ControlIterator = OwnedControlsIterator>
   static void GetControlsWithAttribute(const __ControlIterator begin, const __ControlIterator end,
                                        const std::string &attributeName, std::vector<T *> &foundControls)
      requires std::is_base_of_v<GuiControlBase, T> && std::_Is_iterator_v<__ControlIterator>
   {
      for (auto it = begin; it != end; ++it)
      {
         T *val = dynamic_cast<T *>(*it);

         if (val != nullptr && (*it)->GetAttributes()->AttributeExists(attributeName))
         {
            foundControls.push_back(val);
         }
      }
   }

   template <typename __AttrType, typename __AttrUnderlyingType, typename T = GuiControlBase,
             typename __ControlIterator = OwnedControlsIterator>
   static void GetControlsWithAttributeValue(const __ControlIterator begin, const __ControlIterator end,
                                             const std::string &attributeName,
                                             const __AttrUnderlyingType &attributeValue,
                                             std::vector<T *> &foundControls)
      requires std::is_base_of_v<GuiControlBase, T> && std::is_base_of_v<CtrlAttribute, __AttrType> &&
               EqualityComparable<__AttrUnderlyingType> && std::_Is_iterator_v<__ControlIterator>
   {
      for (auto it = begin; it != end; ++it)
      {
         T *val = dynamic_cast<T *>(*it);

         if (val != nullptr && (*it)->GetAttributes()->AttributeExists(attributeName))
         {
            Attribute *attr = (*it)->GetAttributes()->Get(attributeName);
            if (attr->Is<__AttrType>())
            {
               __AttrType *attrValue = attr->As<__AttrType>();
               static_assert("Underlying attribute type must match the argument type" &&
                             std::is_same_v<decltype(attrValue->Get()), __AttrUnderlyingType>);
               if (attrValue->Get() == attributeValue)
               {
                  foundControls.push_back(val);
               }
            }
         }
      }
   }

   template <typename T = GuiControlBase, typename __ControlIterator = OwnedControlsIterator>
   static T *GetUniqueControlWithType(const __ControlIterator begin, const __ControlIterator end)
      requires std::is_base_of_v<GuiControlBase, T> && std::_Is_iterator_v<__ControlIterator>
   {
      for (auto it = begin; it != end; ++it)
      {
         T *val = dynamic_cast<T *>(*it);
         if (val != nullptr)
         {
            return val;
         }
      }

      return nullptr;
   }

   template <typename T = GuiControlBase, typename __ControlIterator = OwnedControlsIterator>
   static T *GetUniqueControlWithTag(const __ControlIterator begin, const __ControlIterator end, const std::string &tag)
      requires std::is_base_of_v<GuiControlBase, T> && std::_Is_iterator_v<__ControlIterator>
   {
      for (auto it = begin; it != end; ++it)
      {
         T *val = dynamic_cast<T *>(*it);
         if (val != nullptr && val->GetTag() == tag)
         {
            return val;
         }
      }

      return nullptr;
   }

   template <typename T = GuiControlBase, typename __ControlIterator = OwnedControlsIterator>
   static T *GetUniqueControlWithAttribute(const __ControlIterator begin, const __ControlIterator end,
                                           const std::string &attributeName)
      requires std::is_base_of_v<GuiControlBase, T> && std::_Is_iterator_v<__ControlIterator>
   {
      for (auto it = begin; it != end; ++it)
      {
         T *val = dynamic_cast<T *>(*it);

         if (val != nullptr && (*it)->GetAttributes()->AttributeExists(attributeName))
         {
            return val;
         }
      }

      return nullptr;
   }

   template <typename __AttrType, typename __AttrUnderlyingType, typename T = GuiControlBase,
             typename __ControlIterator = OwnedControlsIterator>
   static T *GetUniqueControlWithAttributeValue(const __ControlIterator begin, const __ControlIterator end,
                                                const std::string &attributeName,
                                                const __AttrUnderlyingType &attributeValue)
      requires std::is_base_of_v<GuiControlBase, T> && std::is_base_of_v<CtrlAttribute, __AttrType> &&
               EqualityComparable<__AttrUnderlyingType> && std::_Is_iterator_v<__ControlIterator>
   {
      for (auto it = begin; it != end; ++it)
      {
         T *val = dynamic_cast<T *>(*it);

         if (val != nullptr && (*it)->GetAttributes()->AttributeExists(attributeName))
         {
            Attribute *attr = (*it)->GetAttributes()->Get(attributeName);
            if (attr->Is<__AttrType>())
            {
               __AttrType *attrValue = attr->As<__AttrType>();
               static_assert("Underlying attribute type must match the argument type" &&
                             std::is_same_v<decltype(attrValue->Get()), __AttrUnderlyingType>);
               if (attrValue->Get() == attributeValue)
               {
                  return val;
               }
            }
         }
      }

      return nullptr;
   }
};
} // namespace wgui