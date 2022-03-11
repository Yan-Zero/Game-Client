#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#ifdef _WIN32
#include <WINSOCK2.H>
#pragma comment (lib, "ws2_32.lib")

#include <windows.h>
#endif

#include <stack>
#include <string>

namespace yan
{
  class Socket
  {
  public:
    Socket(int af = -1, int type = -1, int protocol = -1);
    ~Socket();

    bool Connect(std::string ip, int port, int sin_family = -1);
    bool Connect(std::string server);
    bool IsConnected();
    void CloseConnect();

    bool Send(std::string data);
    bool Send(const char *data, int len);
    bool Recv(std::string &data);

  private:
    bool _init;

    std::string ip;
    int port, af, type, protocol;

    #ifdef _WIN32
    SOCKET sock;
    char sz_buffer[1024 + 4];
    #endif

    #ifdef __linux__
    int sock;
    #endif

  };
}

#endif