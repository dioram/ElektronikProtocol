#pragma once
#include <Networking/IServer.h>

class ITCPServer : public IServer {
public:
	virtual Error connect(std::string ip, unsigned int port) = 0;

	virtual ~ITCPServer() {};
};