#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <Networking/Macro.h>
#include <Networking/Error.h>
#include <Networking/IServer.h>

class NETWORKING_EXPORT UDPServer : public IServer
{
private:
	boost::asio::io_service m_ioService; //need for boost socket
	std::shared_ptr<boost::asio::ip::udp::endpoint> m_endPointPtr; //endpoint of destination
	std::shared_ptr<boost::asio::ip::udp::socket> m_udpSocketPtr; //socket which is sending information
	bool m_receive;
protected:
	bool socketIsNull();
	bool endpointIsNull();
	
public:
	UDPServer(bool enableReceive = false);
	Error init(std::string ipAddress, unsigned int port);
	Error sendMessage(const char *data, size_t length);
	Error sendMessage(std::string message);
	Error receiveMessage(char** message, size_t& length);
	inline bool waitForClient(std::chrono::seconds timeout) { return true; };
	//Before using UDPServer you must set endpoint
	Error setEndPoint(std::string ipAddress, UShort port);
	~UDPServer();
};

