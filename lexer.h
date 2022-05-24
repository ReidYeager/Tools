
#ifndef ITools_LEXER_H_
#define ITools_LEXER_H_

#include <string>
#include <string_view> // Used to create sub-strings from the input string

namespace ITools {
  enum TokenTypes
  {
    Token_String,
    Token_Float,
    Token_Decimal, // Decimal-base int
    Token_Hex,     // Hexadecimal-base int (Can use "0x" tag, can not be used with '-')


    Token_Hyphen,
    Token_Comma,
    Token_LeftBracket,
    Token_RightBracket,
    Token_LeftBrace,
    Token_RightBrace,
    Token_LeftParen,
    Token_RightParen,
    Token_FwdSlash,

    Token_NullTerminator,
    Token_LessThan,
    Token_GreaterThan,
    Token_Equal,
    Token_Plus,
    Token_Star,
    Token_BackSlash,
    Token_Pound,
    Token_Period,
    Token_SemiColon,
    Token_Colon,
    Token_Apostrophe,
    Token_Quote,
    Token_Pipe,
    Token_Unknown,

    Token_End,
  };

  struct LexerToken
  {
    ITools::TokenTypes type{};
    std::string string{};
  };

  class Lexer
  {
  private:
    const char* charStream;
    const char* const streamEnd; // Used to avoid requiring \0 at the end of a stream
    const bool usesHex;

  public:
    Lexer(const char* _str, size_t _size, bool _useHex = false) : charStream(_str),
                                                                  streamEnd(_str + _size - 1),
                                                                  usesHex(_useHex)
    {}

    Lexer(const std::vector<char>& _str, bool _useHex = false) : charStream(_str.data()),
                                                                  streamEnd(charStream + _str.size() - 1),
                                                                  usesHex(_useHex)
    {}

    //=========================
    // Token retrieval
    //=========================

    ITools::LexerToken NextToken(bool _expectHex = false, unsigned int _stringLength = 0)
    {
      // Skip whitespace =====
      while(!CompletedStream() && IsWhiteSpace(*charStream))
      {
        charStream++;
      };

      if (CompletedStream())
        return {Token_End, ""};

      // Get token =====
      switch (*charStream)
      {
      case '-':
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        return GetNumberToken(usesHex);
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
      {
        if (_expectHex)
        {
          return GetNumberToken(true);
        }
      }
      case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
      case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
      case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
      case 'Y': case 'Z':
      case '_':
      case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
      case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
      case 's': case 't': case 'u': case 'v': case 'w': case 'x':
      case 'y': case 'z':
        return GetStringToken(_stringLength);
      case ',': return GetSingleCharToken(Token_Comma);
      case '[': return GetSingleCharToken(Token_LeftBracket);
      case ']': return GetSingleCharToken(Token_RightBracket);
      case '{': return GetSingleCharToken(Token_LeftBrace);
      case '}': return GetSingleCharToken(Token_RightBrace);
      case '(': return GetSingleCharToken(Token_LeftParen);
      case ')': return GetSingleCharToken(Token_RightParen);
      case '/': return GetSingleCharToken(Token_FwdSlash);

      case '<': return GetSingleCharToken(Token_LessThan);
      case '>': return GetSingleCharToken(Token_GreaterThan);
      case '=': return GetSingleCharToken(Token_Equal);
      case '+': return GetSingleCharToken(Token_Plus);
      case '*': return GetSingleCharToken(Token_Star);
      case '\\': return GetSingleCharToken(Token_BackSlash);
      case '#': return GetSingleCharToken(Token_Pound);
      case '.': return GetSingleCharToken(Token_Period);
      case ';': return GetSingleCharToken(Token_SemiColon);
      case ':': return GetSingleCharToken(Token_Colon);
      case '\'': return GetSingleCharToken(Token_Apostrophe);
      case '"': return GetSingleCharToken(Token_Quote);
      case '|': return GetSingleCharToken(Token_Pipe);

      case '\0': return GetSingleCharToken(Token_NullTerminator);
      default: return GetSingleCharToken(Token_Unknown);
      }
    }

    // Reads the next token and compares it with the given string
    // Returns true and outputs the read token if they match
    // Returns false if they do not match, Does not move forward in the read stream
    unsigned char ExpectString(std::string _expected, ITools::LexerToken* _outToken = nullptr)
    {
      const char* prevCharHead = charStream;

      ITools::LexerToken next = NextToken(usesHex, _expected.size());

      if (next.string.compare(_expected) == 0)
      {
        if (_outToken != nullptr)
          *_outToken = next;

        return true;
      }

      // Undo token read
      charStream = prevCharHead;
      return false;
    }

    // Reads the next token and compares its type with the given type
    // Returns true and outputs the token if they match
    // Returns false if they do not match, Does not move forward in the read stream
    unsigned char ExpectType(ITools::TokenTypes _expected, ITools::LexerToken* _outToken = nullptr)
    {
      const char* prevCharHead = charStream;

      ITools::LexerToken next = NextToken(_expected == Token_Hex);

      if (next.type == _expected)
      {
        if (_outToken != nullptr)
          *_outToken = next;

        return true;
      }

      // Undo token read
      charStream = prevCharHead;
      return false;
    }

    // Returns true if the read head has reached the end of the read string
    unsigned char CompletedStream()
    {
      return charStream > streamEnd;
    }

    //=========================
    // Numbers
    //=========================

    // Decimal =====

    // Returns an unsigned int
    unsigned int GetUIntFromToken(const ITools::LexerToken* _token)
    {
      int offset = _token->string[0] == '-'; // Ignores negative sign if present
      return (unsigned int)strtoul(_token->string.c_str() + offset, nullptr, 10);
    }

    // Returns a signed int
    int GetIntFromToken(const ITools::LexerToken* _token)
    {
      return (int)strtol(_token->string.c_str(), nullptr, 10);
    }

    // Returns an unsigned long
    unsigned long GetULongFromToken(const ITools::LexerToken* _token)
    {
      int offset = _token->string[0] == '-'; // Ignores negative sign if present
      return strtoul(_token->string.c_str() + offset, nullptr, 10);
    }

    // Returns a signed long
    long GetLongFromToken(const ITools::LexerToken* _token)
    {
      return strtol(_token->string.c_str(), nullptr, 10);
    }

    // Hex =====

    // Returns an unsigned int
    unsigned int GetUIntFromHexToken(const ITools::LexerToken* _token)
    {
      const std::string& str = _token->string;

      // Ignores negative sign if present
      int offset = str[0] == '-';
      // Ignores "0x" if present
      offset += 2 * (str.size() > (2 + offset) && str[0 + offset] == '0' && str[1 + offset] == 'x');

      return (unsigned int)strtoul(_token->string.c_str() + offset, nullptr, 16);
    }

    // Returns a signed int
    int GetIntFromHexToken(const ITools::LexerToken* _token)
    {
      const std::string& str = _token->string;

      // Ignores negative sign if present
      int offset = str[0] == '-';
      // Ignores "0x" if present
      offset += 2 * (str.size() > (2 + offset) && str[0 + offset] == '0' && str[1 + offset] == 'x');

      return (int)strtol(_token->string.c_str() + offset, nullptr, 16);
    }

    // Returns an unsigned long
    unsigned long GetULongFromHexToken(const ITools::LexerToken* _token)
    {
      const std::string& str = _token->string;

      // Ignores negative sign if present
      int offset = str[0] == '-';
      // Ignores "0x" if present
      offset += 2 * (str.size() > (2 + offset) && str[0 + offset] == '0' && str[1 + offset] == 'x');

      return strtoul(_token->string.c_str() + offset, nullptr, 16);
    }

    // Returns a signed long
    long GetLongFromHexToken(const ITools::LexerToken* _token)
    {
      const std::string& str = _token->string;

      // Ignores negative sign if present
      int offset = str[0] == '-';
      // Ignores "0x" if present
      offset += 2 * (str.size() > (2 + offset) && str[0 + offset] == '0' && str[1 + offset] == 'x');

      return strtol(_token->string.c_str() + offset, nullptr, 16);
    }

    // Binary =====

    // Returns an unsigned int
    unsigned int GetUIntFromBinaryToken(const ITools::LexerToken* _token)
    {
      int offset = _token->string[0] == '-'; // Ignores negative sign if present
      return (unsigned int)strtoul(_token->string.c_str() + offset, nullptr, 2);
    }

    // Returns a signed int
    int GetIntFromBinaryToken(const ITools::LexerToken* _token)
    {
      return (int)strtol(_token->string.c_str(), nullptr, 2);
    }

    // Returns an unsigned long
    unsigned long GetULongFromBinaryToken(const ITools::LexerToken* _token)
    {
      int offset = _token->string[0] == '-'; // Ignores negative sign if present
      return strtoul(_token->string.c_str() + offset, nullptr, 2);
    }

    // Returns a signed long
    long GetLongFromBinaryToken(const ITools::LexerToken* _token)
    {
return strtol(_token->string.c_str(), nullptr, 2);
    }

    // Float =====

    // Returns a float
    unsigned int GetFloatFromToken(const ITools::LexerToken* _token)
    {
      return strtof(_token->string.c_str(), nullptr);
    }

    // Returns a double
    unsigned int GetDoubleFromToken(const ITools::LexerToken* _token)
    {
      return strtod(_token->string.c_str(), nullptr);
    }

    //=========================
    // Additional tools
    //=========================

    // Compares the token string with the array of strings
    // Returns [0, _count) as the index of the matching string, _count if no match was found
    unsigned int GetTokenSetIndex(const ITools::LexerToken& _token, const char* const* _stringArray, unsigned int _count)
    {
      unsigned int index;
      for (index = 0; index < _count; index++)
      {
        if (_token.string.compare(_stringArray[index]) == 0)
          return index;
      }

      return index;
    }

  private:
    ITools::LexerToken GetSingleCharToken(ITools::TokenTypes _type)
    {
      return { _type, std::string(std::string_view(charStream++, 1)) };
    }

    ITools::LexerToken GetStringToken(unsigned int _length = 0)
    {
      const char* stringBegining = charStream++;
      unsigned int stringLength = 1;

      if (_length)
      {
        // Get _length number of characters regardless of type
        while (stringLength < _length && !CompletedStream())
        {
          charStream++;
          stringLength++;
        }
      }
      else
      {
        // Find arbirary string up to the first whitespace
        while (IsString(*charStream) && !CompletedStream())
        {
          charStream++;
          stringLength++;
        }
      }

      return { Token_String, std::string(std::string_view(stringBegining, stringLength)) };
    }

    ITools::LexerToken GetNumberToken(bool _isHex = false)
    {
      const char* stringBegining = charStream++;
      unsigned int stringLength = 1;

      int offset = *stringBegining == '-';

      // Handle hyphen-only
      if (offset && !IsNumber(*charStream))
      {
        charStream--;
        return GetSingleCharToken(Token_Hyphen);
      }

      // Hex test =====

      ITools::TokenTypes type = ITools::Token_Decimal;

      // If hex is guaranteed or the '0x' tag is found
      if (_isHex)
      {
        type = ITools::Token_Hex;
      }
      else if (*(stringBegining + offset) == '0' && *(stringBegining + offset + 1) == 'x')
      {
        type = ITools::Token_Hex;

        // Skip the '0x' tag
        charStream = (stringBegining + offset + 2);
        stringLength += 2;
      }

      // Read number =====

      while (IsNumber(*charStream, type) && !CompletedStream())
      {
        if (*charStream == '.')
        {
          if (type == ITools::Token_Hex)
          {
            break;
          }

          type = Token_Decimal;
        }

        charStream++;
        stringLength++;
      }

      return { type, std::string(std::string_view(stringBegining, stringLength)) };
    }

    unsigned char IsWhiteSpace(char _char)
    {
      switch (_char)
      {
      case ' ':
      case '\n':
      case '\r':
      case '\t':
        return true;
      default:
        return false;
      }
    }

    unsigned char IsNumber(char _char, ITools::TokenTypes _type = ITools::Token_Decimal)
    {
      if (_type == ITools::Token_Hex)
      {
        switch (_char)
        {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case 'a': case 'A':
        case 'b': case 'B':
        case 'c': case 'C':
        case 'd': case 'D':
        case 'e': case 'E':
        case 'f': case 'F':
          return true;
        default:
          return false;
        }
      }
      else
      {
        switch (_char)
        {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '.':
          return true;
        default:
          return false;
        }
      }
    }

    unsigned char IsString(char _char)
    {
      switch (_char)
      {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
      case 'A': case 'B': case 'C': case 'D': case 'E':
      case 'F': case 'G': case 'H': case 'I': case 'J':
      case 'K': case 'L': case 'M': case 'N': case 'O':
      case 'P': case 'Q': case 'R': case 'S': case 'T':
      case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
      case '_': case '-':
      case 'a': case 'b': case 'c': case 'd': case 'e':
      case 'f': case 'g': case 'h': case 'i': case 'j':
      case 'k': case 'l': case 'm': case 'n': case 'o':
      case 'p': case 'q': case 'r': case 's': case 't':
      case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
        return true;
      default:
        return false;
      }
    }


  }; // Lexer
} // namespace ITools

#endif // !define ICE_TOOLS_LEXER_H_