#ifndef PARSE_HPP_
#define PARSE_HPP_

#include "lexer.hpp"
#include <iostream>
#include <sstream>

class Parser
{
public:
  Parser(std::string &str) : m_lexer(str) { move(); };
  Parser(Lexer &lexer) : m_lexer(lexer) { move(); };

  inline void move() {
    m_look = m_lexer.scan();
  }
  inline void error(std::string msg) {
    std::string error;
    std::stringstream ss(error);
    ss << "In the " << m_lexer.get_pos() << " : " << msg << std::endl;
    throw std::runtime_error(error);
  }
  inline void match(Token::TokenType type) {
    if(m_look.type() != type)
      error("Expect " + Token::type_to_string(type) + " but " + Token::type_to_string(m_look.type()));
    move();
  }

  // commands -> { command, command* } | { commands }
  void commands();
  // command -> id(params) | id params
  void command();
  // params -> param params | params, param
  void params();
  // param -> string
  void param();
  // id -> [a-zA-Z_][a-zA-Z0-9_]*
  void id();

private:
  Lexer m_lexer;
  Token m_look = Token(Token::TokenType::End);
};

#endif
