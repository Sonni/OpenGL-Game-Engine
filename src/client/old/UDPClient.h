#pragma once

#include "../../shared/UDPConnection.h"

class UDPClient
{
  public:
    UDPClient(char *host, Uint16 port, char *msg)
    {
        UDPConnection udpConn;
        udpConn.initClient(host, port);

        while (true)
        {
            udpConn.udpsend(0, msg);
            udpConn.recieve();
            std::cout << udpConn.in->data << std::endl;
            SDL_Delay(300);
        }
    }

  private:
};