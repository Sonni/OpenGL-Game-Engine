#pragma once

#include <SDL2/SDL_net.h>
#include <iostream>
#include <cstring>
#include <vector>

class TCPClient{
	SDLNet_SocketSet sockets;
	TCPsocket connection;
	char data[1400];
	public:
	TCPClient(const char* ip, Uint16 port);
	~TCPClient();
	void start();
	void send(const std::string& msg);
	void recive();
	void disconnect();
	void checkInput();
};
