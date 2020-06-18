#pragma once
#if defined _WIN32 || defined _WIN64
#include <SDKDDKVer.h>
#endif
#include <iostream>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <Networking/export.h>

#include <Networking/Macro.h>
#include <Networking/Error.h>
#include <Networking/ITCPServer.h>

class NETWORKING_EXPORT TCPServer : public ITCPServer
{
public:
	TCPServer();
	TCPServer(short port);
	Error init(std::string ipAddress, unsigned int port) override;
	Error connect(std::string ip, unsigned int port) override;
	bool waitForClient(std::chrono::seconds timeout) override;
	void start();
	void stop();
	Error sendMessage(const char *data, size_t length) override;
	Error sendMessage(std::string message) override;
	Error receiveMessage(char** message, size_t& length) override;
	~TCPServer();

private:
    typedef std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;

	void ListenThreadFunc();
	void ListenForConnections();
	void HandleAccept(socket_ptr socket, const boost::system::error_code& err);
	boost::asio::ip::tcp::endpoint ParseEndpoint(std::string ip, unsigned int port);

    std::shared_ptr<std::thread> m_acceptorThread;
	std::vector<socket_ptr> m_acceptedConnections;
	std::shared_ptr<boost::asio::io_service> m_service;
	std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
	socket_ptr m_receiveSocketPtr; //socket which is sending information
	bool m_cancel = false;
	std::mutex m_lockConnections;
	volatile bool m_newConnectionAccepted = false;
};