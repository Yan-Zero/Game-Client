#include "socket.hpp"
#include "lexer.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <signal.h>
#include <thread>
#include "client_command.h"
#include "ui.hpp"
#include <mutex>

// 7000ms
#define BEAT_INTERVAL 7000

using namespace std;

void thread_handler();
string ip, name;
int port;
long long id;
auto sock = yan::Socket();
thread t;
std::mutex mtx; // 保护 cout
#ifdef _WIN32
HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

std::map<std::string, void (*)(std::string &)> func_map = {
  {"exit", &exit_program},
  {"quit", &exit_program},
  {"q", &exit_program},
  {"help", &help},
  {"set", &set},
  {"connect", &_connect},
  {"room", &room},
  {"info", &info},
  {"error", &error},
  {"show_message", &show_message},
  {"show_help", &show_help},
};


void help(string &arg) {
  if(arg == "set")
  {
    cout << "set:" << endl
         << "    set name <name>: set your name" << endl
         << "    set server <ip>:<port>: set server" << endl
         << "    set ip <server ip>: set server ip" << endl
         << "    set port <server port>: set server port" << endl;
  }
  elif(arg == "info")
  {
    cout << "info:" << endl
         << "    info:                     show your info" << endl
         << "    info room [room id]:      show room info" << endl
         << "    info player [player id]:  show player info" << endl
         << "    info <player id>:         show player info" << endl;
  }
  elif(arg == "help")
  {
    cout << "help:" << endl
       << "    help:           show the help" << endl
       << "    help <command>: show the help of <command>" << endl;
  }
  elif(arg == "")
  {
    cout << "connect:  try to connect to server" << endl
         << "exit:     exit the game" << endl
         << "help:     show the all help" << endl
         << "info:     show the info. Please type 'help info' to see more." << endl
         << "message:  send message" << endl
         << "player:   list all players in room" << endl
         << "room:     the command of room. Please type 'help room' to see more." << endl
         << "game:     show the game info. Please type 'help game' to see more." << endl;
  }
  else
    sock.Send("help " + arg);
  cout << endl;
}
void exit_program(string &arg) {
  id = -2;
  if(arg != "-no_save")
  {
    ofstream config("config.txt");
    config << ip << ":" << port << endl
           << name << endl;
  }
  else
    std::cout << "bye~" << std::endl;
  arg = "exit";
  sock.Send(arg);
  mtx.unlock();
  sock.~Socket();
  exit(0);
}
void set(string &arg)
{
  Lexer lexer(arg);
  auto arg_ = lexer.get_next();
  auto value = lexer.get_next();
  if(arg_.type() == Token::TokenType::kEnd)
  {
    arg = "set";
    help(arg);
  }
  elif(value.type() == Token::TokenType::kEnd)
  {
    cout << "Please input the value." << endl;
    return;
  }
  elif(arg_.value() == "name")
  {
    name = value.value();
    transform(name.begin(), name.end(), name.begin(), [](char c) { 
      if (c == ' ') return '-';
      elif (c == '\t') return '-';
      elif (c == '\n') return '-';
      elif (c == '\r') return '-';
      elif (c == '\f') return '-';
      elif (c == '\v') return '-';
      else return c;
    });
    cout << "name set to " << name << endl;
    if(lexer.get_next().value() != "-f")
      sock.Send("rename " + name);
  }
  elif(arg_.value() == "server")
  {
    ip = value.value().substr(0, value.value().find(":"));
    port = stoi(value.value().substr(value.value().find(":") + 1));
  }
  elif(arg_.value() == "ip")
    ip = value.value();
  elif(arg_.value() == "port")
    port = stoi(value.value());
  elif(arg_.value() == "__id__")
    id = stoll(value.value());
  else
    cout << "set: unknown command" << endl;
}
void room(string &arg) {
  Lexer token(arg);
  auto tk = token.get_next();
  if(tk.value() == "list")
    sock.Send("list room");
  else
    sock.Send("room " + arg);
}
void _connect(string &arg) {
  sock.Connect(ip, port);
  // 如果用了 utf-8，记得改
  sock.Send("encode gbk");
  sock.Recv(arg);
  if(arg == "OK")
  {
    sock.Send("rename " + name + " -s");
    t = thread(thread_handler);
  }
  else
  {
    cout << "recv data: " << arg << endl;
    cout << "connect to " << ip << ":" << port << " failed" << endl;
    sock.CloseConnect();
    id = -1;
  }
}
void info(string &arg) {
  if(arg == "")
  {
    cout << "id:   " << id << endl
         << "name: " << name << endl;
    return;
  }
  Lexer lexer(arg);
  auto tk = lexer.get_next();
  if(tk.value() == "room")
    sock.Send("info " + arg);
  elif(tk.value() == "player")
    sock.Send("info " + arg);
  else
    sock.Send("info player " + to_string(id));
}


void deal_command_from_server(string &arg)
{
  Lexer lexer(arg);
  auto command = lexer.get_next();
  if(command.type() == Token::TokenType::kEnd)
    return;
  auto it = func_map.find(command.value());
  if(it == func_map.end())
  {
    cout << "Unknown command: " << command.value() << endl;
    return;
  }
  arg = arg.substr(lexer.get_pos());
  it->second(arg);
}
void thread_handler() {
  string arg, str;
  short x, y;
  if(!id)
    sock.Send("info player");
  while(sock.IsConnected() && id != -2)
  {
    sock.Recv(arg);
    mtx.lock();
    Get_Console_Cursor_Position(x, y);
    str = Read_Console_Line(y, x, hConsoleOutput);
    Move_Console_Cursor_To_Line_Begin(hConsoleOutput);
    deal_command_from_server(arg);
    cout << str;
    mtx.unlock();
  }
  return;
}
void sigint_handler(int sig) {
  if(sig == SIGINT){
    string arg = "-no_save";
    exit_program(arg);
  }
}


int main(int, char**) {
  signal(SIGINT, sigint_handler);

  // read the config.txt file
  ifstream config("config.txt");
  if (!config) {
    cerr << "Could not open config.txt" << endl;
    return 1;
  }
  // read the frist line as sever ip
  config >> ip;
  port = atoi(ip.substr(ip.find(':') + 1).c_str());
  ip = ip.substr(0, ip.find(':'));
  // read the second line as name
  config >> name;
  if(name == "")
  {
    cout << "please enter your name: ";
    cin >> name;
  }
  config.close();
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

  thread _t([](){
    string command, param;
    _connect(param);

    while(id != -2) {
      mtx.lock();
      cout << name << ": ";
      mtx.unlock();

      getline(cin, command);
      command = string_trim(command);
      if(command == "")
        continue;
      if(command.find(" ") != string::npos)
      {
        param = command.substr(command.find(" ") + 1);
        command = command.substr(0, command.find(" "));
      }
      else
        param = "";
      transform(command.begin(), command.end(), command.begin(), ::tolower);

      mtx.lock();
      if(func_map.find(command) != func_map.end())
        func_map[command](param);
      else
        cout << "command \"" << command << "\" not found" << endl;
      mtx.unlock();
    }
  });
  _t.join();
}
