#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include <vector>
#include <string>
#define elif else if

inline std::string string_trim(const std::string &str) {
  std::string ret = str;
  return ret.erase(0, ret.find_first_not_of(" \t\n\r\f\v")).erase(ret.find_last_not_of(" \t\n\r\f\v") + 1); 
}

class Token
{
public:
  enum class TokenType {
    End,
    String,
    LeftBracket,
    RightBracket, 
    LeftParenthesis,
    RightParenthesis,
    Comma,
  };

  Token(TokenType type) : m_type(type) {}
  Token(const Token &other) : m_type(other.m_type), m_value(other.m_value) {}
  Token(const std::string &str, TokenType type) : m_value(str), m_type(type) {};
  Token(const std::string &str) : m_value(str), m_type(TokenType::String) {};
  inline TokenType type() const { return m_type; }
  inline std::string value() const { return m_value; }
  inline static std::string type_to_string(TokenType type) {
    switch(type) {
      case TokenType::End: return "End";
      case TokenType::String: return "String";
      case TokenType::LeftBracket: return "LeftBracket";
      case TokenType::RightBracket: return "RightBracket";
      case TokenType::LeftParenthesis: return "LeftParenthesis";
      case TokenType::RightParenthesis: return "RightParenthesis";
      case TokenType::Comma: return "Comma";
      default: return "Unknown";
    }
  }
  inline bool operator==(const Token &other) const {
    return m_type == other.m_type && m_value == other.m_value;
  }
  inline bool operator==(const TokenType &other) const {
    return m_type == other;
  }
  inline bool operator==(const std::string &other) const {
    return m_type == TokenType::String && m_value == other;
  }
  inline bool operator!=(const Token &other) const {
    return !(*this == other);
  }
  inline bool operator!=(const TokenType &other) const {
    return !(*this == other);
  }
  inline bool operator!=(const std::string &other) const {
    return !(*this == other);
  }

private:
  std::string m_value;
  TokenType m_type;
};

class Lexer
{
public:
  Lexer(const std::string &str) {
    m_str = string_trim(str);
    m_pos = 0;
  };
  Lexer(const Lexer &other) {
    m_str = other.m_str;
    m_pos = other.m_pos;
    m_ch = other.m_ch;
  }

  Token scan();
  size_t get_pos() { return m_pos; }

private:
  std::string m_str;
  size_t m_pos;
  char m_ch;

  inline void ignore_space()
  {
    while(m_ch == ' '  || m_ch == '\t' || m_ch == '\n' 
       || m_ch == '\r' || m_ch == '\f' || m_ch == '\v')
      read_char();
  }
  std::string get_string();
  inline void read_char()
  {
    if (m_pos >= m_str.size())
      m_ch = '\0';
    else
      m_ch = m_str[m_pos];
    m_pos++;
  }
  inline bool read_char(char c)
  {
    read_char();
    if(m_ch != c)
      return false;
    m_ch = ' ';
    return true;
  }

};

#endif