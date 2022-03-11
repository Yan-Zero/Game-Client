#include "lexer.hpp"
#include "parser.hpp"
using namespace std;

Token Lexer::scan()
{
  ignore_space();
  if(m_ch == '\0')
    return Token(Token::TokenType::End);
  switch (m_ch)
  {
    case '\"':
    case '\'': return Token(get_string(), Token::TokenType::String);
    case '(':  return Token(Token::TokenType::LeftParenthesis);
    case ')':  return Token(Token::TokenType::RightParenthesis);
    case ',':  return Token(Token::TokenType::Comma);
    case '{':  return Token(Token::TokenType::LeftBracket);
    case '}':  return Token(Token::TokenType::RightBracket);
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
            default:
              ret += '\\';
              ret += m_ch;
              break;
            case '\0': ret += '\\';
              return Token(ret, Token::TokenType::String);
            case '\"': ret += '\"'; break;  
            case '\'': ret += '\''; break;
            case '\\': ret += '\\'; break;
            case 'n':  ret += '\n'; break;
            case 'r':  ret += '\r'; break;
            case 't':  ret += '\t'; break;
            case 'v':  ret += '\v'; break;
            case '{':  ret += '{';  break;
            case '}':  ret += '}';  break;
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
        default:
          ret += '\\';
          ret += m_ch;
          break;
        case '\0': ret += '\\'; return ret;
        case '\"': ret += '\"'; break;
        case '\'': ret += '\''; break;
        case '\\': ret += '\\'; break;
        case 'n':  ret += '\n'; break;
        case 'r':  ret += '\r'; break;
        case 't':  ret += '\t'; break;
        case 'v':  ret += '\v'; break;
      }
    }
    else
      ret += m_str[m_pos];
    read_char();
  }
  return ret;
}

void Parser::commands()
{
  match(Token::TokenType::LeftBracket);
  // { commands }
  if(m_look == Token::TokenType::LeftBracket)
    commands();
  // { command, command* }
  else
  {
    command();
    while(m_look == Token::TokenType::Comma)
    {
      match(Token::TokenType::Comma);
      command();
    }
  }
  match(Token::TokenType::RightBracket);
}

void Parser::command()
{
  id();
  // id(params)
  if(m_look == Token::TokenType::LeftParenthesis)
  {
    match(Token::TokenType::LeftParenthesis);
    params();
    match(Token::TokenType::RightParenthesis);
  }
  // id params
  else
    params();
}

void Parser::params()
{
  // param | param params
  if(m_look == Token::TokenType::String)
  {
    param();
    params();
  }
  elif(m_look == Token::TokenType::Comma)
  {
    match(Token::TokenType::Comma);
    param();
    params();
  }
  return;
}

void Parser::param()
{
  match(Token::TokenType::String);
}

void Parser::id()
{
  auto id = m_look;
  match(Token::TokenType::String);
  char ch = id.value()[0];
  if(!isalpha(ch) && ch != '_')
    error("id must start with a letter or underscore");
  for(size_t i = 1; i < id.value().size(); ++i)
  {
    ch = id.value()[i];
    if(!isalnum(ch) && ch != '_')
      error("id must contain only letters, numbers, or underscores");
  }
}