#include "UDPConnection.h"

UDPConnection::UDPConnection()
{
}

UDPConnection::~UDPConnection()
{
    SDLNet_FreePacket(out);
    SDLNet_FreePacket(in);
    SDLNet_FreePacketV(packets);
}

int UDPConnection::initServer(Uint16 port)
{
    /* open udp server socket */
    if (!(sock = SDLNet_UDP_Open(port)))
    {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        exit(4);
    }
    printf("port %hd opened\n", port);

    /* allocate max packet */
    if (!(out = SDLNet_AllocPacket(65535)))
    {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        exit(5);
    }
    if (!(in = SDLNet_AllocPacket(65535)))
    {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        exit(5);
    }

    /* allocate 32 packets of size len */
    if (!(packets = SDLNet_AllocPacketV(32, 65535)))
    {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        exit(6);
    }
}

int UDPConnection::initClient(char *host, Uint16 port)
{
    this->port = port;
    this->host = host;
    if (SDLNet_ResolveHost(&ip, host, port) == -1)
    {
        printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(4);
    }

    /* open udp client socket */
    if (!(sock = SDLNet_UDP_Open(0)))
    {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        exit(5);
    }

    /* allocate max packet */
    if (!(out = SDLNet_AllocPacket(65535)))
    {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        exit(6);
    }
    if (!(in = SDLNet_AllocPacket(65535)))
    {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        exit(6);
    }

    /* bind server address to channel 0 */
    if (SDLNet_UDP_Bind(sock, 0, &ip) == -1)
    {
        printf("SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
        exit(7);
    }
}

void UDPConnection::udpsend(int channel, const char *message)
{
    sprintf(msg, "%s", message);
    strcpy((char *)out->data, msg);
    out->len = strlen(msg) + 2;

    if (!SDLNet_UDP_Send(sock, channel, out))
    {
        printf("SDLNet_UDP_Send: %s\n", SDLNet_GetError());
        exit(1);
    }
}

int UDPConnection::udpRequest(UDPsocket sock, int channel, UDPpacket *out, UDPpacket *in, Uint32 delay, Uint8 expect, int timeout)
{
    Uint32 t, t2;
    int err;

    in->data[0] = 0;
    t = SDL_GetTicks();
    do
    {
        t2 = SDL_GetTicks();
        if (t2 - t > (Uint32)timeout)
        {
            printf("timed out\n");
            return (0);
        }
        if (!SDLNet_UDP_Send(sock, channel, out))
        {
            printf("SDLNet_UDP_Send: %s\n", SDLNet_GetError());
            exit(1);
        }
        err = SDLNet_UDP_Recv(sock, in);
        if (!err)
            SDL_Delay(delay);
    } while (!err || (in->data[0] != expect && in->data[0] != ERROR));
    if (in->data[0] == ERROR)
        printf("received error code\n");
    return (in->data[0] == ERROR ? -1 : 1);
}

int UDPConnection::udprecv(UDPsocket sock, UDPpacket *in, Uint32 delay, Uint8 expect, int timeout)
{
    Uint32 t, t2;
    int err;

    in->data[0] = 0;
    t = SDL_GetTicks();
    do
    {
        t2 = SDL_GetTicks();
        if (t2 - t > (Uint32)timeout)
        {
            /*printf("timed out\n"); // this is commented to look nicer... */
            return (0);
        }
        err = SDLNet_UDP_Recv(sock, in);
        if (!err)
            SDL_Delay(delay);
    } while (!err || (in->data[0] != expect && in->data[0] != ERROR));
    if (in->data[0] == ERROR)
        printf("received error code\n");
    return (in->data[0] == ERROR ? -1 : 1);
}

void UDPConnection::recieve()
{

    while (!SDLNet_UDP_Recv(sock, in))
        SDL_Delay(100);

    memcpy(&ip, &in->address, sizeof(IPaddress));
}

void UDPConnection::bind()
{
    if (SDLNet_UDP_Bind(sock, 0, &ip) == -1)
    {
        printf("SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
        exit(7);
    }
}

void UDPConnection::unbind()
{
    SDLNet_UDP_Unbind(sock, 0);
}

void UDPConnection::printPacketInfo()
{
    const char *host = SDLNet_ResolveIP(&ip);
    Uint32 ipnum = SDL_SwapBE32(ip.host);
    port = SDL_SwapBE16(ip.port);
    if (host)
        printf("Message from host=%s port=%hd: %s\n", host, port, in->data);
    else
        printf("Message from host=%d.%d.%d.%d port=%hd: %s\n",
               ipnum >> 24,
               (ipnum >> 16) & 0xff,
               (ipnum >> 8) & 0xff,
               ipnum & 0xff,
               port, in->data);

}