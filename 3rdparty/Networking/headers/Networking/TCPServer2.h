#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <memory>
#include <thread>
#include <mutex>
#include <Networking/Error.h>
#include <Networking/IServer.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "ws2_32")

class NETWORKING_EXPORT TCPServer2 : public IServer {
private:
	WSADATA m_wsaData;
	u_long m_iMode = 1; //non-blocking socket
	SOCKET m_serverSocket = INVALID_SOCKET;
	SOCKET m_clientSocket = INVALID_SOCKET;
	bool m_destroyFlag = false;
	std::shared_ptr<std::thread> m_clientChecker;
	bool m_clientConnected = false;
	std::mutex m_lockSocket;

	void resetState();
	void checkClient();

public:
	Error init(std::string ipAddress, unsigned int port);
	Error sendMessage(const char* message, size_t length);
	Error sendMessage(std::string message);
	inline Error receiveMessage(char** message, size_t& length) { return Error::invalid; };
	bool waitForClient(std::chrono::seconds timeout);
	~TCPServer2();
};