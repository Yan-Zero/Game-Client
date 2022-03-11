#include "lexer.hpp"
using namespace std;

Token Lexer::get_next_token()
{
  ignore_space();
  if(pos_ >= str_.size())
    return Token(Token::TokenType::EOC);
  switch (str_[pos_])
  {
  case '\"':
  case '\'':
    return Token(get_string(), Token::TokenType::String);
  case '{':
    pos_++;
    return Token(Token::TokenType::LeftBracket);
  case '}':
    pos_++;
    return Token(Token::TokenType::RightBracket);
  case '(':
    pos_++;
    return Token(Token::TokenType::LeftParenthesis);
  case ')':
    pos_++;
    return Token(Token::TokenType::RightParenthesis);
  case ',':
    pos_++;
    return Token(Token::TokenType::Comma);
  default:
    string ret;
    while(pos_ < str_.size() 
       && str_[pos_] != ' ' && str_[pos_] != '\t'
       && str_[pos_] != '\n' && str_[pos_] != '\r'
       && str_[pos_] != '\f' && str_[pos_] != '\v'
       && str_[pos_] != '{' && str_[pos_] != '}')
    {
      if(str_[pos_] == '\\')
      {
        ++pos_;
        if(pos_ >= str_.size())
          break;
        if(str_[pos_] == 'n')
          ret += '\n';
        elif(str_[pos_] == 't')
          ret += '\t';
        elif(str_[pos_] == 'r')
          ret += '\r';
        elif(str_[pos_] == '{')
          ret += '{';
        elif(str_[pos_] == '}')
          ret += '}';
        elif(str_[pos_] == '\\')
          ret += '\\';
        else
          ret += str_[pos_];
      }
      else
        ret += str_[pos_];
      ++pos_;
    }
    return Token(ret, Token::TokenType::String);
  }
}

string Lexer::get_string()
{
  string ret;
  char ch = str_[pos_];
  while(++pos_ < str_.length() && str_[pos_] != ch)
    if(str_[pos_] == '\\')
    {
      pos_++;
      if(pos_ >= str_.length())
        return ret;
      switch(str_[pos_])
      {
        case 'n':
          ret += '\n';
          break;
        case 't':
          ret += '\t';
          break;
        case 'r':
          ret += '\r';
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
          ret += str_[pos_];
          break;
      }
    }
    else
      ret += str_[pos_];
  ++pos_;
  return ret;
}
