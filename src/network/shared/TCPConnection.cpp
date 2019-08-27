#include "TCPConnection.h"
#include <iostream>

/*
UDPConnection udpConnection;
TCPConnection server(nullptr, 4444);
bool loopInput = true;

std::vector<std::string> splitString(std::string strToSplit, char delimeter)
{
	std::stringstream ss(strToSplit);
	std::string item;
	std::vector<std::string> splittedStrings;
	while (std::getline(ss, item, delimeter))
	{
		splittedStrings.push_back(item);
	}
	return splittedStrings;
}

void checkInput()
{
	while (loopInput)
	{
		std::string input;
		std::getline(std::cin, input);
		std::vector<std::string> split = splitString(input, ' ');
		server.sendMsg(split[1], std::stoi(split[0]));
	}
}
*/

TCPConnection::TCPConnection(Uint16 port)
{
    IPaddress ip;

    SDLNet_ResolveHost(&ip, NULL, port);
    sockets = SDLNet_AllocSocketSet(30); // max player
    if (sockets == NULL)
    {
        fprintf(stderr, "Couldn't create socket set: %s\n", SDLNet_GetError());
        closeTCPConnection();
        return;
    }

    serverSocket = SDLNet_TCP_Open(&ip);
    if (serverSocket == NULL)
    {
        fprintf(stderr, "Couldn't create TCPConnection socket: %s\n", SDLNet_GetError());
        closeTCPConnection();
        return;
    }
    std::cout << "Success!" << std::endl;
}

TCPConnection::~TCPConnection()
{
}


void TCPConnection::sendMsg(const std::string &msg, const TCPsocket &socket)
{
    SDLNet_TCP_Send(socket, msg.c_str(), msg.size() + 1);
}

void TCPConnection::sendMsg(const std::string &msg, int index)
{
    if (index < clients.size())
        SDLNet_TCP_Send(clients[index], msg.c_str(), msg.size() + 1);
}

void TCPConnection::sendToAll(const std::string &msg)
{
    for (int i = 0; i < clients.size(); i++)
    {
        sendMsg(msg, clients.at(i));
    }
}

void TCPConnection::disconnectClient(int clientIndex)
{
    std::cout << "Disconnecting client " << clientIndex << std::endl;
    sendMsg("You have been disconnected by the TCPConnection", clients[clientIndex]);
    SDLNet_TCP_DelSocket(sockets, clients[clientIndex]);
    SDLNet_TCP_Close(clients[clientIndex]);
    clients.erase(clients.begin() + clientIndex);
}

void TCPConnection::closeTCPConnection()
{
    std::cout << "Closing the TCPConnection" << std::endl;
    for (int i = clients.size() - 1; i >= 0; i--)
    {
        disconnectClient(i);
    }

    if (serverSocket != NULL)
    {
        SDLNet_TCP_Close(serverSocket);
        serverSocket = NULL;
    }

    if (sockets != NULL)
    {
        SDLNet_FreeSocketSet(sockets);
        sockets = NULL;
    }
}