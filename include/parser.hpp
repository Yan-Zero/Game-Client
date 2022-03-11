#ifndef PARSE_HPP_
#define PARSE_HPP_

#include "lexer.hpp"
#include <iostream>
#include <sstream>
#include <vector>

class PCommand
{
public:
  PCommand() {};
  PCommand(const std::string &func) : m_func_name(func) {};

  inline void AddParam(const Token &arg)
  { m_args.push_back(Token(arg)); }
  void AddParams(const std::vector<Token> &args) { 
    for(auto &arg : args)
      m_args.push_back(arg);
  }
  inline std::string GetCommand() const
  { return m_func_name + " " + GetArgs(); }
  std::string GetArgs() const
  {
    std::string ret;
    for(auto &arg : m_args)
    {
      ret += "\"";
      for (auto &c : arg.value())
        if (c == '"')
          ret += "\\\"";
        else
          ret += c;
      ret += "\" ";
    }
    return ret;
  }
  void SetFuncName(const std::string &func) { m_func_name = func; }
  inline const std::string& GetFuncName() const { return m_func_name; }
  inline const std::vector<Token>& GetArgs_v() const { return m_args; }
  operator std::string() const { return GetCommand(); }

private:
  std::string m_func_name;
  std::vector<Token> m_args = std::vector<Token>();
};

class Parser
{
public:
  std::vector<PCommand*> PCMDs;

  Parser(const std::string &str) : m_lexer(str) { move(); };
  Parser(Lexer &lexer) : m_lexer(lexer) { move(); };

  ~Parser()
  {
    for (auto &cmd : PCMDs)
      delete cmd;
  }

  inline void move() {
    m_look = m_lexer.scan();
  }
  inline void error(std::string msg) {
    std::stringstream ss;
    ss << "In the " << m_lexer.get_pos() << " : " << msg << std::endl;
    throw std::runtime_error(ss.str());
  }
  inline void match(Token::TokenType type) {
    if(m_look != type)
      error("Expect " + Token::type_to_string(type) + " but " + Token::type_to_string(m_look.type()));
    move();
  }
  inline void parse() {
    PCMDs.clear();
    stmt();
  }

  // block -> { stmt stmt* }
  void block();
  // stmt -> block block* | command (, command)*
  void stmt();
  // command -> id(params) | id params
  void command();
  // // params_comma -> param (, params)
  // void params_comma();
  // // params_space -> param params
  // void params_space();
  // params -> params_comma | params_space | ϵ | param
  inline void params();
  // param -> string
  void param();
  // id -> [a-zA-Z_][a-zA-Z0-9_]*
  void id();

private:
  Lexer m_lexer;
  Token m_look = Token(Token::TokenType::End);

  PCommand* m_pcmd = nullptr;
};

#endif
