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
#include <parser.hpp>

// 7000ms
#define BEAT_INTERVAL 7000

using namespace std;

void thread_handler();
auto sock = Client();
string ip, name;
int port, id;
thread t;
std::mutex mtx; // 保护 cout
#ifdef _WIN32
HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

map<string, void (*)(const vector<Token> &)> func_map = {
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
  {"update_help", &update_help},
  {"player", &player_list},
  {"print", &print},
  {"disconnect", &disconnect}
};
extern map<string, string> cmd_list;

void _connect(const vector<Token> &args) {
  if(id != 0)
  {
    cout << "You are already connected to server." << endl;
    return;
  }
  sock.Connect(ip, port);
  #ifdef _WIN32
  // 一般而言， Win32 是 GBK， Linux 是 UTF-8 //
  sock.Send("encode gbk");
  #endif
  #ifdef __linux__
  sock.Send("encode utf-8");
  #endif

  string data;
  sock.Recv(data);
  if(data == "OK")
  {
    id = 0;
    sock.Send("rename " + name + " -s");
    t = thread(thread_handler);
  }
  else
  {
    cout << "recv data: " << data << "EOF" << endl;
    cout << "connect to " << ip << ":" << port << " failed" << endl;
    sock.CloseConnect();
    id = -1;
  }
}
void exit_program(const vector<Token> &args) {
  id = -2;
  bool no_save = false;
  for (auto &i : args)
    if(i == "-no_save")
      no_save = true;
  if(!no_save)
  {
    ofstream config("config.txt");
    config << ip << ":" << port << endl
           << name << endl;
  }
  std::cout << "bye~" << std::endl;
  sock.Send("exit");
  mtx.unlock();
  exit(0);
}
void deal_command(const string &arg)
{
  // cout << "deal_command: " << arg << endl;
  Parser parser(arg);
  try { parser.parse(); }
  catch (std::exception &e)
  { error({Token(e.what())}); }

  for(auto &cmd : parser.PCMDs)
  {
    auto func_name = cmd->GetFuncName();
    transform(func_name.begin(), func_name.end(), func_name.begin(), ::tolower);
    auto func = func_map.find(func_name);
    if(func == func_map.end())
    {
      if(cmd_list.find(func_name) != cmd_list.end())
        sock.Send(arg);
      else
        cout << "unknown command: " << func_name << endl;
    }
    else
      func->second(cmd->GetArgs_v());
  }
}
void thread_handler() {
  string arg, str;
  short x, y;
  while(sock.IsConnected() && id >= 0)
  {
    sock.Recv(arg);
    mtx.lock();
    Get_Console_Cursor_Position(x, y, hConsoleOutput);
    str = Read_Console_Line(y, x, hConsoleOutput);
    Move_Console_Cursor_To_Line_Begin(hConsoleOutput);
    deal_command(arg);
    cout << str;
    mtx.unlock();
  }
  sock.CloseConnect();
  id = 0;
  return;
}
void sigint_handler(int sig) {
  if(sig == SIGINT)
    exit_program({Token("-no_save")});
}

int main(int, char**) {
  signal(SIGINT, sigint_handler);

  #ifdef _WIN32
  cout << "If you have the problme in print Chinese, please use gbk encoding." << endl;
  cout << "Type 'chcp 936' in cmd/powershell to change the encoding to gbk." << endl;
  #endif

  // read the config.txt file
  ifstream config("config.txt");
  if (config) {
    // read the frist line as sever ip
    config >> ip;
    port = atoi(ip.substr(ip.find(':') + 1).c_str());
    ip = ip.substr(0, ip.find(':'));
    // read the second line as name
    config >> name;
    config.close();
  }
  else {
    cerr << "Could not open config.txt" << endl;
    ip = "127.0.0.1";
    port = 2333;
  }
  
  name = string_trim(name);
  if(name.empty())
  {
    cout << "please enter your name: ";
    getline(cin, name);
  }
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
    _connect({});

    while(id != -2) {
      mtx.lock();
      cout << name << ": ";
      mtx.unlock();

      getline(cin, command);
      command = string_trim(command);
      if(command == "")
        continue;

      mtx.lock();
      deal_command(command);
      mtx.unlock();
    }
  });
  _t.join();
  t.join();
}
