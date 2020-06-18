#include <Networking/TCPServer2.h>
#include <functional>

#include <assert.h>

TCPServer2::~TCPServer2() {
	m_destroyFlag = true;
	if (m_clientChecker != nullptr && m_clientChecker->joinable())
		m_clientChecker->join();
}

Error TCPServer2::init(std::string ipAddress, unsigned int port) {
	struct sockaddr_in server;
	printf("\nInitializing Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) {
		printf("Failed. Error Code: %d", WSAGetLastError());
		return Error::UDPSocketWasntCreated;
	}
	printf("Initialized.\n");

	if ((m_serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket: %d", WSAGetLastError());
		return Error::UDPSocketWasntCreated;
	}
	printf("Socket created.\n");

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	if (bind(m_serverSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("Bind failed with error code: %d", WSAGetLastError());
		return Error::UDPSocketWasntCreated;
	}
	printf("Bind done.\n");

	printf("Listening...\n");
	if (listen(m_serverSocket, 1) == SOCKET_ERROR) {
		printf("Could not listen: %d", WSAGetLastError());
		return Error::UDPSocketWasntCreated;
	}

	if (ioctlsocket(m_serverSocket, FIONBIO, &m_iMode) != NO_ERROR) {
		printf("Server ioctlsocket failed");
		return Error::UDPSocketWasntCreated;
	}

	int send_buffer = 64 * 1024; // 64 KB 
	int send_buffer_sizeof = sizeof(int);
	setsockopt(m_serverSocket, SOL_SOCKET, SO_SNDBUF, (char*)&send_buffer, send_buffer_sizeof);
	m_clientChecker = std::make_shared<std::thread>(std::bind(&TCPServer2::checkClient, this));
	return Error::success;
}

void TCPServer2::checkClient() {
	while (!m_destroyFlag)
	{
		while (!m_clientConnected)
		{
			SOCKET tmpSocket = SOCKET_ERROR;
			tmpSocket = accept(m_serverSocket, NULL, NULL);
			int send_buffer = 64 * 1024; // 64 KB 
			int send_buffer_sizeof = sizeof(int);
			setsockopt(tmpSocket, SOL_SOCKET, SO_SNDBUF, (char*)&send_buffer, send_buffer_sizeof);
			if (tmpSocket != SOCKET_ERROR) {
				printf("Connection accepted.\n");
				m_clientSocket = tmpSocket;
				if (ioctlsocket(m_clientSocket, FIONBIO, &m_iMode) != NO_ERROR) {
					printf("Client ioctlsocket failed");
					return;
				}
				m_clientConnected = true;
			}
		}
	}
}

Error TCPServer2::sendMessage(const char* message, size_t length) {
	int error;
	size_t r;
	{
		std::lock_guard<std::mutex> socketGuard(m_lockSocket);
		r = send(m_clientSocket, message, length, 0);
	}
	error = WSAGetLastError();
	if (error == WSAEWOULDBLOCK) return Error::invalid;
	if (error != 0 || r <= 1) {
		resetState();
		return Error::invalid;
	}
	assert(r == length);
	return Error::success;
}

void TCPServer2::resetState() {
	shutdown(m_clientSocket, SD_SEND);
	closesocket(m_clientSocket);
	m_clientConnected = false;
}

Error TCPServer2::sendMessage(std::string message) {
	return sendMessage(message.data(), message.size());
}

bool TCPServer2::waitForClient(std::chrono::seconds timeout) {
	auto whenToStop = std::chrono::system_clock::now() + timeout;
	while (std::chrono::system_clock::now() < whenToStop && !m_clientConnected)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return m_clientConnected;
}
