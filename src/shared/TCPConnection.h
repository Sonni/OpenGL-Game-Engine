#pragma once

#include <SDL2/SDL_net.h>
#include <vector>
#include <string>

class TCPConnection
{
  public:
    TCPConnection(Uint16 port);
    ~TCPConnection();

    void start();

    void sendMsg(const std::string &msg, int index);

    std::vector<TCPsocket> clients;
    SDLNet_SocketSet sockets;
    TCPsocket serverSocket;

    void sendMsg(const std::string &msg, const TCPsocket &socket);
    void sendToAll(const std::string &msg);
    void disconnectClient(int clientIndex);
    void closeTCPConnection();
  private:
};