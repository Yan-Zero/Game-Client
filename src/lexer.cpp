#include "lexer.hpp"
using namespace std;

Token Lexer::scan()
{
  ignore_space();
  if(m_ch == '\0')
    return Token(Token::TokenType::End);
  switch (m_ch)
  {
  case '\"':
  case '\'':
    return Token(get_string(), Token::TokenType::String);
  case '{':
    return Token(Token::TokenType::LeftBracket);
  case '}':
    return Token(Token::TokenType::RightBracket);
  case '(':
    return Token(Token::TokenType::LeftParenthesis);
  case ')':
    return Token(Token::TokenType::RightParenthesis);
  case ',':
    return Token(Token::TokenType::Comma);
  default:
    string ret;
    while(m_ch != ' '  && m_ch != '\t' && m_ch != '\n' 
       && m_ch != '\r' && m_ch != '\f' && m_ch != '\v'
       && m_ch != '{'  && m_ch != '}'  && m_ch != '('
       && m_ch != ')'  && m_ch != ','  && m_ch != '\0')
    {
      if(m_ch == '\\')
      {
        read_char();
        switch(m_ch)
        {
          case '\0':
            ret += '\\';
            return Token(ret, Token::TokenType::String);
          case '\"':
            ret += '\"';
            break;
          case '\'':
            ret += '\'';
            break;
          case '\\':
            ret += '\\';
            break;
          case 'n':
            ret += '\n';
            break;
          case 'r':
            ret += '\r';
            break;
          case 't':
            ret += '\t';
            break;
          case 'v':
            ret += '\v';
            break;
          case '{':
            ret += '{';
            break;
          case '}':
            ret += '}';
            break;
          default:
            ret += '\\';
            ret += m_ch;
            break;
        }
      }
      else
        ret += m_ch;
      read_char();
    }
    return Token(ret, Token::TokenType::String);
  }
}

string Lexer::get_string()
{
  string ret;
  char ch = m_ch;
  read_char();
  while(ch != '\0' && m_ch != ch)
  {
    if(m_ch == '\\')
    {
      read_char();
      switch(m_ch)
      {
        case '\0':
          ret += '\\';
          return ret;
        case 'n':
          ret += '\n';
          break;
        case 't':
          ret += '\t';
          break;
        case 'r':
          ret += '\r';
          break;
        case 'v':
          ret += '\v';
          break;
        case '\\':
          ret += '\\';
          break;
        case '\'':
          ret += '\'';
          break;
        case '\"':
          ret += '\"';
          break;
        default:
          ret += '\\';
          ret += m_ch;
          break;
      }
    }
    else
      ret += m_str[m_pos];
    read_char();
  }
  return ret;
}
