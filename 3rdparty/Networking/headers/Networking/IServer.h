#pragma once

#include <Networking/Error.h>
#include <string>
#include <chrono>

class IServer
{
public:
	virtual Error init(std::string ipAddress, unsigned int port) = 0;
	virtual Error sendMessage(const char* message, size_t length) = 0;
	virtual Error sendMessage(std::string message) = 0;
	virtual bool waitForClient(std::chrono::seconds timeout) = 0;
	virtual Error receiveMessage(char** receivedMessage, size_t& sizeOfMessage) = 0;

	virtual ~IServer() {};
};
