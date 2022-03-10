#include <iostream>
#include "lexer.hpp"
#include "socket.hpp"
#include <WS2tcpip.h>

using namespace std;

Token Lexer::get_next()
{
  ignore_space();
  if(pos_ >= str_.size())
    return Token(Token::TokenType::kEnd);
  string ret;
  if(str_[pos_] == '"' || str_[pos_] == '\'')
    return Token(get_string(), Token::TokenType::kString);
  else
    ret = str_.substr(pos_, str_.find(" ", pos_) - pos_);
  pos_ += ret.size() + 1;
  return Token(ret);
}

string Lexer::get_string()
{
  string ret;
  char ch = str_[pos_];
  while(++pos_ < str_.length() && str_[pos_] != ch)
    if(str_[pos_] == '\\')
    {
      pos_++;
      if(pos_ >= str_.length())
        return ret;
      switch(str_[pos_])
      {
        case 'n':
          ret += '\n';
          break;
        case 't':
          ret += '\t';
          break;
        case 'r':
          ret += '\r';
          break;
        case '\\':
          ret += '\\';
          break;
        case '\'':
          ret += '\'';
          break;
        case '\"':
          ret += '\"';
          break;
        default:
          ret += '\\';
          ret += str_[pos_];
          break;
      }
    }
    else
      ret += str_[pos_];
  ++pos_;
  return ret;
}

#ifdef _WIN32

yan::Socket::Socket(int af, int type, int protocol)
{
  if(!_init)
  {
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);
    if(WSAStartup(wVersionRequested, &wsaData) != 0)
    {
      cerr << "WSAStartup failed" << endl;
      return;
    }
    _init = true;
  }

  if(af < 0)
    af = AF_INET;
  if(type < 0)
    type = SOCK_STREAM;
  if(protocol < 0)
    protocol = IPPROTO_TCP;
  this->af = af;
  this->type = type;
  this->protocol = protocol;
  sock = socket(af, type, protocol);
}

bool yan::Socket::Connect(string ip, int port, int sin_family)
{
  if(sin_family < 0)
    sin_family = af;
  if(sock == INVALID_SOCKET)
    sock = socket(af, type, protocol);
  sockaddr_in sockAddr;
  memset(&sockAddr, 0, sizeof(sockAddr));
  sockAddr.sin_family = sin_family;
  sockAddr.sin_addr.s_addr = inet_addr(ip.c_str());
  sockAddr.sin_port = htons(port);
  return connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) != SOCKET_ERROR;
}

bool yan::Socket::Recv(string &data)
{
  memset(sz_buffer, 0, sizeof(sz_buffer));
  int ret = recv(sock, sz_buffer, sizeof(sz_buffer), 0);
  if(ret < 0)
  {
    closesocket(sock);
    sock = INVALID_SOCKET;
    return false;
  }
  data = sz_buffer;
  return true;
}

bool yan::Socket::Send(string data)
{
  return Send(data.c_str(), (int)data.size());
}

bool yan::Socket::Send(const char *data, int len_)
{
  if (sock != INVALID_SOCKET)
    return send(sock, data, len_, 0);
  cerr << "Socket is not connected" << endl;
  return false;
}

yan::Socket::~Socket()
{
  closesocket(sock);
}

bool yan::Socket::IsConnected()
{
  char buffer[1];
  recv(sock, buffer, 1, MSG_PEEK);
  return WSAECONNRESET != WSAGetLastError();
}

void yan::Socket::CloseConnect()
{
  closesocket(sock);
  sock = INVALID_SOCKET;
}

#endif

bool yan::Socket::Connect(string server)
{
  return Connect(server.substr(0, server.find(":")), atoi(server.substr(server.find(":") + 1).c_str()), -1);
}
