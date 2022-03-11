#include <iostream>
#include "socket.hpp"
#include "lexer.hpp"
#include "client_command.h"

using namespace std;


void error(string &arg) {
  Lexer lexer(arg);
  auto tk = lexer.get_next_token();
  while(tk.type() != Token::TokenType::EOC)
  {
    cout << "Error: " << tk.value() << endl;
    tk = lexer.get_next_token();
  }
}
void show_message(string &arg) {
  Lexer lexer(arg);
  cout << "Message From " << lexer.get_next_token().value() << ": " << endl;
  auto token = lexer.get_next_token();
  while(token.type() != Token::TokenType::EOC)
  {
    cout << " |" << token.value() << endl;
    token = lexer.get_next_token();
  }
  cout << endl;
}
void show_help(string &arg) {
  Lexer lexer(arg);
  auto token = lexer.get_next_token();
  while(token.type() != Token::TokenType::EOC)
  {
    token = lexer.get_next_token();
    cout << token.value() << endl;
  }
}
