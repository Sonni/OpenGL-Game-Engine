#pragma once

#include "SDL2/SDL_net.h"
#include <cstring>

#include <iostream>
#include <fstream>
#include <sstream>

#define TIMEOUT (5000) /*five seconds */
#define ERROR (0xff)

class UDPConnection
{
  public:
    UDPConnection();
    ~UDPConnection();

    int initServer(Uint16 por);
    int initClient(char *host, Uint16 port);

    void udpsend(int channel, const char *message);
    int udpRequest(UDPsocket sock, int channel, UDPpacket *out, UDPpacket *in, Uint32 delay, Uint8 expect, int timeout);
    int udprecv(UDPsocket sock, UDPpacket *in, Uint32 delay, Uint8 expect, int timeout);

    void recieve();

    void bind();
    void unbind();

    void printPacketInfo();

    char *host;
    Uint16 port;
    IPaddress ip;
    UDPsocket sock;
    UDPpacket *out, *in, **packets, *outs[32];
    char msg[65535];

  private:
};