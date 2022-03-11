#include <iostream>
#include "socket.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <algorithm>
#include "client_command.h"

using namespace std;

extern Client sock;
map<std::string, std::string> cmd_list = {
};
extern std::string name, ip;
extern int port;
extern long long id;

void help(const std::vector<Token> &args) {
  if(!args.size())
  {
    const string list[] = {
      "connect:  try to connect to server",
      "exit:     exit the game",
      "help:     show the all help",
      "info:     show the info. Please type 'help info' to see more.",
      "message:  send message",
      "player:   list all players in room",
      "game:     show the game info. Please type 'help game' to see more."
    };
    for (auto &cmd : list)
      cout << cmd << endl;
    for (auto &cmd : cmd_list)
      cout << cmd.second << endl;
    cout << endl;
  }
  else for (auto &i : args)
  {
    if(i != Token::TokenType::String)
      continue;
    auto arg = i.value();
    if(arg == "set")
    {
      cout << "set:" << endl
          << "    set name <name>: set your name" << endl
          << "    set server <ip>:<port>: set server" << endl
          << "    set ip <server ip>: set server ip" << endl
          << "    set port <server port>: set server port" << endl << endl;
    }
    elif(arg == "info")
    {
      cout << "info:" << endl
          << "    info:                     show your info" << endl
          << "    info room [room id]:      show room info" << endl
          << "    info player [player id]:  show player info" << endl
          << "    info <player id>:         show player info" << endl << endl;
    }
    elif(arg == "help")
    {
      cout << "help:" << endl
        << "    help:           show the help" << endl
        << "    help <command>: show the help of <command>" << endl << endl;
    }
    else
      sock.Send("help " + arg);
  }
}
void error(const std::vector<Token> &args) {
  for (auto &i : args)
  {
    if(i != Token::TokenType::String)
      continue;
    cout << "Error: " << i.value() << endl;
  }
}
void show_message(const std::vector<Token> &args) {
  auto token = args.begin();
  cout << "Message From " << (token++)->value() << ": " << endl;
  while(token != args.end())
  {
    cout << " |" << token->value() << endl;
    ++token;
  }
  cout << endl;
}
void show_help(const std::vector<Token> &args) {
  for(auto &arg : args)
    cout << arg.value() << endl;
}
void set(const std::vector<Token> &args) {
  if(args.size() < 2)
  {
    cout << "set: too few arguments" << endl;
    return;
  }
  elif(args[0] == "name")
  {
    name = args[1].value();
    transform(name.begin(), name.end(), name.begin(), [](char c) { 
      if (c == ' ') return '-';
      elif (c == '\t') return '-';
      elif (c == '\n') return '-';
      elif (c == '\r') return '-';
      elif (c == '\f') return '-';
      elif (c == '\v') return '-';
      elif (c == '\b') return '-';
      else return c;
    });
    cout << "name set to " << name << endl;
    for (auto &i : args)
      if (i == "-f")
        return;
    sock.Send("rename " + name);
  }
  elif(args[0] == "server")
  {
    ip = args[1].value().substr(0, args[1].value().find(":"));
    port = stoi(args[1].value().substr(args[1].value().find(":") + 1));
  }
  elif(args[0] == "ip")
    ip = args[1].value();
  elif(args[0] == "port")
    port = stoi(args[1].value());
  elif(args[0] == "__id__")
    id = stoll(args[1].value());
  else
    cout << "set: unknown command \"" << args[0].value() << "\"" << endl << endl;
}
void room(const std::vector<Token> &args) {
  if(args.size() < 1)
    help({Token("room")});
  elif(args[0] == "list")
      sock.Send("list room");
  else
  {
    PCommand cmd("room");
    cmd.AddParams(args);
    sock.Send(cmd);
  }
}
void info(const std::vector<Token> &args) {
  if(args.size() == 0)
  {
    cout << "id:   " << id << endl
         << "name: " << name << endl;
    return;
  }
  if(args[0] == "room" || args[0] == "player")
  {
    PCommand cmd("info");
    cmd.AddParams(args);
    sock.Send(cmd);
  }
  else
    sock.Send("info player " + to_string(id));
}
void update_help(const std::vector<Token> &args)
{
  if(args.size() % 2)
  {
    cout << "Update Help: too few arguments" << endl;
    return;
  }
  for(auto i = args.begin(); i < args.end(); )
    cmd_list[i->value()] = (++i)->value();
}