#include "XmlFile.h"

namespace
{
   bool IsWhiteSpace(char ch) 
   {
      return ch == ' ' || ch == '\n' || ch == '\r' || ch == 9;
   }

   bool IsAlpha(char ch) 
   {
      return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
   }

   bool IsNum(char ch) 
   {
      return ch >= '0' && ch <= '9';
   }

   bool IsAlphaNum(char ch) 
   {
      return IsAlpha(ch) || IsNum(ch);
   }

   enum class eTokenTypes
   {
      None,
      // <?
      HeaderOpen,
      // ?>
      HeaderClose,
      // <
      TagOpen,
      // />
      TagClose,
      // "..."
      String,
      // =
      Equals,
      // "quot 'apos &amp >gt <lt
      SpecialCharacter
   };

   enum class eSpecialCharacterCodes
   {
      Quote,
      Apostrophe,
      Ampersand,
      GreaterThan,
      LessThan
   };

   struct Token
   {
      int Code;
      std::string Lexeme;
      eTokenTypes Type;
   };

   class XmlTokenizer
   {
   public:
      /**
      bool GetNextToken(Token& ret) 
      {
         SkipWhiteSpace(xml, index, size);
         char ch = xml[index];

         if (index >= size) {
            ret.begin = nullptr;
            ret.end = nullptr;
            return false;
         }

         if (loadEscapeSequence(xml, index, size, &ret)) {
         }
         else if (loadTwoChar(xml, index, size, &ret)) {
         }
         else if (loadReserve(xml, index, size, &ret)) {
         }
         else {
            loadWord(xml, index, size, &ret);
         }

         if (ret.Code == -1)
         {
            return false;
         }

         return true;
      }
      */
   private:
      static constexpr int MaxBufferSize = 4096;
      char mCurrentBuffer[MaxBufferSize] = { 0 };

      int mCurrentBufferLength = 0;
      int mIndexInBuffer = 0;

      char mChar;

      int mCurrentLine = 1;
      int mCurrentChar = 0;

      std::istream mInputStream;

      char NextCharacter()
      {
         if (mIndexInBuffer < mCurrentBufferLength)
         {
            mChar = mCurrentBuffer[mIndexInBuffer++];
            AdvanceLineAndChar(mChar);
            return true;
         }

         if (!LoadNextBuffer())
         {
            mChar = EOF;
            return false;
         }

         mIndexInBuffer = 0;
         mChar = mCurrentBuffer[mIndexInBuffer++];
         AdvanceLineAndChar(mChar);

         return true;
      }

      void AdvanceLineAndChar(char mChar)
      {
         // Adjust line and column counters as needed.
         // Moves to a new line if \n is found.
         if (mChar == '\n')
         {
            mCurrentLine++;
            mCurrentChar = 0;
         }
         else
         {
            mCurrentChar++;
         }
      }

      bool LoadNextBuffer()
      {
         if (mInputStream.eof())
         {
            mIsEof = true;
            return false;
         }

         try
         {
            mInputStream.read(mCurrentBuffer, MaxBufferSize);
            mCurrentBufferLength = mInputStream.gcount();
         }
         catch (std::exception e)
         {
            std::cout << e.what() << "\n";
            mIsEof = true;
            return false;
         }

         return true;
      }

      void SkipWhiteSpace() 
      {
         while (IsWhiteSpace(mChar) && !mIsEof) 
         {
            NextCharacter();
         }

         // Handle whitespace and recursively call back into skip whitespace.
      }

      bool mIsEof = false;
   };
}

namespace xml
{

}