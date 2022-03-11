#ifndef CLIENT_COMMAND_HPP_
#define CLIENT_COMMAND_HPP_

#include <string>
#include <map>

void help(std::string &arg);
void exit_program(std::string &arg);
void set(std::string &arg);
void room(std::string &arg);
void _connect(std::string &arg);
void info(std::string &arg);
void error(std::string &arg);
void show_message(std::string &arg);
void show_help(std::string &arg);
void update_help(std::string &arg);

#endif