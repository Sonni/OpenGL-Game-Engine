#include "TCPClient.h"
#include <thread>

void TCPClient::checkInput()
{
    while (true)
    {
		char input[100];
		std::cin.getline(input, sizeof(input));
		send(std::string(input));
    }
}

TCPClient::TCPClient(const char *ipaddress, Uint16 port)
{
    //std::thread t1(&TCPClient::checkInput, this);

	SDLNet_Init();
	IPaddress ip;
	if (SDLNet_ResolveHost(&ip, ipaddress, port) == -1)
		std::cout << "error while resolving the host (bad ip?)" << std::endl;
	connection = SDLNet_TCP_Open(&ip);
	sockets = SDLNet_AllocSocketSet(1);
	if (sockets == NULL)
		std::cout << "error while connecting (bad ip?)" << std::endl;
	SDLNet_TCP_AddSocket(sockets, connection);

	std::cout << "Connected to ip: " << ipaddress << " at port: " << port << std::endl;
}

void TCPClient::start()
{
    bool run = true;
    while (run)
    {
        SDLNet_CheckSockets(sockets, 10);

        if (SDLNet_SocketReady(connection))
        {

            int res = SDLNet_TCP_Recv(connection, data, 1400);
            if (res > 0)
            {
                std::string d(data);
                std::cout << "Got: " << data << std::endl;
            }
            else
            {
                std::cout << "Server DC'ed" << std::endl;
                run = false;
            }
        }

        SDL_Delay(300);
    }
}

TCPClient::~TCPClient()
{
	disconnect();
	SDLNet_TCP_Close(connection);
	SDLNet_FreeSocketSet(sockets);
}

void TCPClient::disconnect()
{
	SDLNet_TCP_Send(connection, "-1", 3); //close with the server
	std::cout << "Disconnected from server" << std::endl;
}

void TCPClient::send(const std::string &msg)
{
	SDLNet_TCP_Send(connection, msg.c_str(), msg.size() + 1);
	//std::cout << "sent: " << "'" << msg << "'" << std::endl;
}

void TCPClient::recive()
{
	SDLNet_CheckSockets(sockets, 10);
	if (SDLNet_SocketReady(connection))
	{
		int offset = SDLNet_TCP_Recv(connection, (char *)data + (offset), 1400);
		if (offset <= 0)
			return;
		std::cout << "got: " << data << std::endl;
	}
}
