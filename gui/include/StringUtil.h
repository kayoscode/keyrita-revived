#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

/// <summary>
/// Basic utility functions for string processing.
/// </summary>
class StringUtil {
public:
   /// <summary>
   /// Splits a string at the given character and returns it in the provided vector.
   /// </summary>
   /// <param name="string"></param>
   /// <param name="splitChar"></param>
   /// <param name="ret"></param>
   static void SplitString(const std::string& string, char splitChar, std::vector<std::string>& ret) 
   {
      ret.clear();
      std::string toPush("");

      for (int i = 0; i < string.size(); i++) 
      {
         if (string[i] == splitChar) 
         {
            if (toPush.size() != 0) 
            {
               ret.push_back(toPush);
               toPush = "";
            }
         }
         else 
         {
            toPush += string[i];
         }
      }

      if (toPush != "") 
      {
         ret.push_back(toPush);
      }
   }

   /// <summary>
   /// Given the file name, get the extension of the file.
   /// </summary>
   /// <param name="name"></param>
   /// <param name="dest"></param>
   /// <returns></returns>
   static bool GetFileExtension(const std::string& name, std::string& dest) 
   {
      int i = (int)name.size() - 1;

      //search backward till we find a dot or a special character that cannot be in a file extension 
      while (name[i] != '.' && i >= 0) 
      {
         if (name[i] == '/' || name[i] == '\\') 
         {
            return false;
         }

         i--;
      }

      if (i == 0 && name[0] != '.') 
      {
         //no file extension
         return false;
      }

      //everything up until the dot mustve been the file extension
      dest = std::string(name.begin() + i + 1, name.end());

      // Return the extensionin upper case.
      std::for_each(dest.begin(), dest.end(), [](char& c) 
         {
            c = std::toupper(c);
         });

      return true;
   }
};