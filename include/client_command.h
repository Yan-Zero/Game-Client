#ifndef CLIENT_COMMAND_HPP_
#define CLIENT_COMMAND_HPP_

#include <map>
#include <vector>
#include "lexer.hpp"

void help(const std::vector<Token> &arg);
void exit_program(const std::vector<Token> &arg);
void set(const std::vector<Token> &arg);
void room(const std::vector<Token> &arg);
void _connect(const std::vector<Token> &arg);
void info(const std::vector<Token> &arg);
void error(const std::vector<Token> &arg);
void show_message(const std::vector<Token> &arg);
void show_help(const std::vector<Token> &arg);
void update_help(const std::vector<Token> &arg);
void player_list(const std::vector<Token> &args);
void print(const std::vector<Token> &args);
void disconnect(const std::vector<Token> &args);

#endif