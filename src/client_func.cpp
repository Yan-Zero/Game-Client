#include <iostream>
#include "socket.hpp"
#include "lexer.hpp"
#include "client_command.h"

using namespace std;


void error(string &arg) {
  Lexer lexer(arg);
  auto tk = lexer.scan();
  while(tk.type() != Token::TokenType::End)
  {
    cout << "Error: " << tk.value() << endl;
    tk = lexer.scan();
  }
}
void show_message(string &arg) {
  Lexer lexer(arg);
  cout << "Message From " << lexer.scan().value() << ": " << endl;
  auto token = lexer.scan();
  while(token.type() != Token::TokenType::End)
  {
    cout << " |" << token.value() << endl;
    token = lexer.scan();
  }
  cout << endl;
}
void show_help(string &arg) {
  Lexer lexer(arg);
  auto token = lexer.scan();
  while(token.type() != Token::TokenType::End)
  {
    token = lexer.scan();
    cout << token.value() << endl;
  }
}
