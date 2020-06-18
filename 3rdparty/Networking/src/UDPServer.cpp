#include <Networking/UDPServer.h>

using namespace boost::asio;

Error UDPServer::setEndPoint(std::string ipAddress, UShort port)
{
	ip::address_v4 address;
	std::transform(ipAddress.begin(), ipAddress.end(), ipAddress.begin(), ::tolower);
	if (ipAddress.compare("localhost") == 0)
	{
		ipAddress = "127.0.0.1";
	}
	boost::system::error_code er;
	address = ip::address_v4::from_string(ipAddress, er);
	if (er == error::invalid_argument)
	{
		return Error::wrongIPAddress;
	}
	m_endPointPtr = std::make_shared<ip::udp::endpoint>(address, port);
	return Error::success;
}

bool UDPServer::socketIsNull()
{
	return m_udpSocketPtr == NULL;
}

bool UDPServer::endpointIsNull()
{
	return m_endPointPtr == NULL;
}

UDPServer::UDPServer(bool enableReceive) :
	m_receive(enableReceive)
{
}

Error UDPServer::init(std::string ipAddress, unsigned int port)
{
	Error err = setEndPoint(ipAddress, port);
	m_udpSocketPtr = std::make_shared<ip::udp::socket>(m_ioService);
	m_udpSocketPtr->open(ip::udp::v4());
	boost::system::error_code errStub;
	if (m_receive)
		m_udpSocketPtr->bind(*m_endPointPtr, errStub);
	return err;
}

Error UDPServer::sendMessage(const char *data, size_t length)
{
	if (socketIsNull())
		return Error::UDPSocketWasntCreated;
	if (endpointIsNull())
		return Error::endpointWasntSet;
	size_t sentBytes = m_udpSocketPtr->send_to(
		buffer(data, length),
		*m_endPointPtr
	);
	assert(sentBytes == length);
	return Error::success;
}

Error UDPServer::sendMessage(std::string message)
{
	return sendMessage(message.data(), message.length());
}

Error UDPServer::receiveMessage(char** message, size_t& length)
{
	*message = nullptr;
	length = 0;
	if (socketIsNull())
		return Error::UDPSocketWasntCreated;
	if (endpointIsNull())
		return Error::endpointWasntSet;
	if (m_udpSocketPtr->available() > 0)
	{
		char messageBuffer[1024];
		std::memset(messageBuffer, 0, sizeof(messageBuffer));
		length = m_udpSocketPtr->receive_from(
			buffer(messageBuffer, 1024),
			*m_endPointPtr);
		*message = new char[length];
		std::memcpy(*message, &messageBuffer[0], length);
		return Error::success;
	}
	return Error::invalid;
}


UDPServer::~UDPServer()
{
	if (m_udpSocketPtr != nullptr)
	{
		m_udpSocketPtr->close();
		m_udpSocketPtr = nullptr;
	}
	m_endPointPtr = nullptr;
	m_ioService.stop();
}
