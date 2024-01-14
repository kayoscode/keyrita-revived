#pragma once
#include <cassert>
#include <map>
#include <string>

namespace wgui
{
constexpr int Flag(int bit)
{
   assert(bit < 32 && bit > 0);
   return 1 << (bit - 1);
}

struct CaseInsensitiveStrCompare
{
 public:
   bool operator()(const std::string &a, const std::string &b) const
   {
      return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(),
                                          [](char a, char b) { return std::tolower(a) < std::tolower(b); });
   }
};

enum class eWindowFlags
{
   Header = Flag(7),
   ScrollbarAutoHide = Flag(8),
   InBackground = Flag(9),
};

class WindowFlagsUtil
{
 public:
   /// <summary>
   /// Gets the flag for a given string. -1 if not found.
   /// </summary>
   /// <param name="text"></param>
   /// <returns></returns>
   static int GetFlagFor(const std::string &text)
   {
      const auto &foundItem = GetFlags().find(text);

      if (foundItem != GetFlags().end())
      {
         return static_cast<int>(foundItem->second);
      }

      return -1;
   }

 private:
   static const std::map<std::string, eWindowFlags, CaseInsensitiveStrCompare> &GetFlags()
   {
      static const std::map<std::string, eWindowFlags, CaseInsensitiveStrCompare> flagValues{
         {"Header", eWindowFlags::Header},
         {"ScrollbarAutoHide", eWindowFlags::ScrollbarAutoHide},
         {"InBackground", eWindowFlags::InBackground},
      };

      return flagValues;
   }
};

enum class eTextAlignmentFlags
{
   Left = Flag(1),
   Center = Flag(2),
   Right = Flag(3),
   Top = Flag(4),
   VerticalCenter = Flag(5),
   Bottom = Flag(6),

   CenterLeft = VerticalCenter | Left,
   FullyCentered = VerticalCenter | Center,
   CenterRight = VerticalCenter | Right,
};

class TextAlignFlagsUtil
{
 public:
   /// <summary>
   /// Gets the flag for a given string. -1 if not found.
   /// </summary>
   /// <param name="text"></param>
   /// <returns></returns>
   static int GetFlagFor(const std::string &text)
   {
      const auto &foundItem = GetFlags().find(text);

      if (foundItem != GetFlags().end())
      {
         return static_cast<int>(foundItem->second);
      }

      return -1;
   }

 private:
   static const std::map<std::string, eTextAlignmentFlags, CaseInsensitiveStrCompare> &GetFlags()
   {
      static const std::map<std::string, eTextAlignmentFlags, CaseInsensitiveStrCompare> flagValues{
         {"Left", eTextAlignmentFlags::Left},
         {"Center", eTextAlignmentFlags::Center},
         {"Right", eTextAlignmentFlags::Right},
         {"Top", eTextAlignmentFlags::Top},
         {"VerticalCenter", eTextAlignmentFlags::VerticalCenter},
         {"Bottom", eTextAlignmentFlags::Bottom},

         {"CenterLeft", eTextAlignmentFlags::CenterLeft},
         {"FullyCentered", eTextAlignmentFlags::FullyCentered},
         {"CenterRight", eTextAlignmentFlags::CenterRight},
      };

      return flagValues;
   }
};
} // namespace wgui