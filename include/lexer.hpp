#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include <string>
#define elif else if

inline std::string string_trim(std::string &str) {
  return str.erase(0, str.find_first_not_of(" \t\n\r\f\v")).erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
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
  Token(std::string str, TokenType type) : m_str(str), m_type(type) {};
  inline TokenType type() { return m_type; }
  inline std::string value() { return m_str; }
  inline bool operator==(const Token &other) {
    return m_type == other.m_type && m_str == other.m_str;
  }
  inline bool operator==(const TokenType &other) {
    return m_type != TokenType::String && m_type == other;
  }
  inline bool operator==(const std::string &other) {
    return m_type == TokenType::String && m_str == other;
  }

private:
  std::string m_str;
  TokenType m_type;
};

class Lexer
{
public:
  Lexer(std::string &str) {
    m_str = string_trim(str);
    m_pos = 0;
  };

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