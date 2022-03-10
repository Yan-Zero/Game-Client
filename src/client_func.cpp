#include <iostream>
#include "socket.hpp"
#include "lexer.hpp"
#include "client_command.h"

using namespace std;


void error(string &arg) {
  Lexer lexer(arg);
  auto tk = lexer.get_next();
  while(tk.type() != Token::TokenType::kEnd)
  {
    cout << "Error: " << tk.value() << endl;
    tk = lexer.get_next();
  }
}
void show_message(string &arg) {
  Lexer lexer(arg);
  cout << "Message From " << lexer.get_next().value() << ": " << endl;
  auto token = lexer.get_next();
  while(token.type() != Token::TokenType::kEnd)
  {
    cout << " |" << token.value() << endl;
    token = lexer.get_next();
  }
  cout << endl;
}
void show_help(string &arg) {
  Lexer lexer(arg);
  auto token = lexer.get_next();
  while(token.type() != Token::TokenType::kEnd)
  {
    token = lexer.get_next();
    cout << token.value() << endl;
  } 
}
