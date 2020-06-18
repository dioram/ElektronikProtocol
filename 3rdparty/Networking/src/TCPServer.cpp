#include <Networking/TCPServer.h>

#include <boost/chrono.hpp>
#include <boost/timer/timer.hpp>

using namespace boost::asio::ip;

TCPServer::TCPServer() 
	: m_service(nullptr), m_acceptor(nullptr) {
}

TCPServer::TCPServer(short port) {
	std::cout << __FUNCTION__ << std::endl;
	tcp::endpoint ep(tcp::v4(), port);
	init(ep.address().to_v4().to_string(), port);
}

boost::asio::ip::tcp::endpoint TCPServer::ParseEndpoint(std::string _ip, unsigned int port) {
	std::cout << __FUNCTION__ << std::endl;
	std::transform(_ip.begin(), _ip.end(), _ip.begin(),
		[](unsigned char c) { return std::tolower(c); });
	m_service = std::make_shared<boost::asio::io_service>();
	std::transform(_ip.begin(), _ip.end(), _ip.begin(),
		[](unsigned char c) { return std::tolower(c); });
	if (_ip == "localhost") {
		_ip = "127.0.0.1";
	}
	auto ip = boost::asio::ip::address::from_string(_ip);
	return tcp::endpoint(ip, port);
}

Error TCPServer::connect(std::string ipAddress, unsigned int port) {
	if (m_receiveSocketPtr != nullptr) {
		if (m_receiveSocketPtr->is_open()) {
			m_receiveSocketPtr->shutdown(tcp::socket::shutdown_both);
			m_receiveSocketPtr->close();
		}
	}
	tcp::endpoint ep = ParseEndpoint(ipAddress, port);
	m_receiveSocketPtr = std::make_shared<tcp::socket>(*m_service);
	m_receiveSocketPtr->open(tcp::v4());
	boost::system::error_code ec;
	m_receiveSocketPtr->connect(ep, ec);
	if (ec.failed())
		std::cout << "connection FAILED" << std::endl;
	m_service->run();
	if (ec.value() == 0)
		return Error::success;
	else
		return Error::invalid;
}

Error TCPServer::init(std::string ipAddress, unsigned int port) {
	tcp::endpoint ep = ParseEndpoint(ipAddress, port);
	m_acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(*m_service, ep);
	start();
	return Error::success;
}

void TCPServer::ListenThreadFunc() {
	std::cout << __FUNCTION__ << std::endl;
	ListenForConnections();
	m_service->run();
}

void TCPServer::ListenForConnections() {
	std::cout << __FUNCTION__ << std::endl;
	socket_ptr newConnection = std::make_shared<tcp::socket>(*m_service);
	m_acceptor->async_accept(*newConnection, 
		boost::bind(&TCPServer::HandleAccept, this, 
			newConnection, boost::asio::placeholders::error));
}

void TCPServer::HandleAccept(socket_ptr socket, const boost::system::error_code& err) {
	std::cout << __FUNCTION__ << std::endl;
	if (err) {
		std::cout << "Error accepting connection: " << err.message() << std::endl;
		return;
	}
	{
		std::lock_guard<std::mutex> lock(m_lockConnections);
		m_acceptedConnections.push_back(socket);
		m_newConnectionAccepted = true;
	}
	if (!m_cancel) {
		ListenForConnections();
	}
}

Error TCPServer::sendMessage(const char *data, size_t length) {
    std::lock_guard<std::mutex> lock(m_lockConnections);
    if (m_acceptedConnections.size() > 0) {
		for (auto connection : m_acceptedConnections) {
			boost::system::error_code error;
			size_t transferred_bytes = boost::asio::write(*connection, boost::asio::buffer(data, length), error);
			if (!error) {
			    assert(length == transferred_bytes);
			}
			else {
			    return Error::sendError;
			}
		}
	}
	return Error::success;
}

Error TCPServer::sendMessage(std::string message) {
	return sendMessage(message.data(), message.length());
}

Error TCPServer::receiveMessage(char** message, size_t& length) {
	*message = nullptr;
	length = length <= 0 ? 1024 : length;
	if (m_receiveSocketPtr == nullptr)
		return Error::UDPSocketWasntCreated;
	if (m_receiveSocketPtr->available() > 0)
	{
		std::vector<char> messageBuffer(length);
		std::memset(messageBuffer.data(), 0, sizeof(char) * length);
		length = m_receiveSocketPtr->receive(
			boost::asio::buffer(messageBuffer, length));
		*message = new char[length];
		std::memcpy(*message, &messageBuffer[0], length);
		return Error::success;
	}
	return Error::invalid;
}

TCPServer::~TCPServer() {
	stop();
}

bool TCPServer::waitForClient(std::chrono::seconds timeout) {
	std::cout << __FUNCTION__ << std::endl;
	boost::timer::cpu_timer t;
	boost::chrono::duration<float> elapsed = boost::chrono::nanoseconds(t.elapsed().user);
	while (elapsed.count() < timeout.count()) {
		if (m_newConnectionAccepted) {
			m_newConnectionAccepted = false;
			return true;
		}
	}
	bool accepted = m_newConnectionAccepted;
	m_newConnectionAccepted = false;
	return accepted;
}

void TCPServer::start() {
	if (!m_cancel) {
		m_cancel = false;
		m_acceptorThread = std::make_shared<std::thread>(std::bind(&TCPServer::ListenThreadFunc, this));
	}
}

void TCPServer::stop() {
	std::lock_guard<std::mutex> lock(m_lockConnections);
	m_cancel = true;
	if (m_service != nullptr) {
		for (auto& acceptedConnection : m_acceptedConnections) {
			acceptedConnection->shutdown(boost::asio::socket_base::shutdown_both);
			acceptedConnection->close();
		}
		m_acceptedConnections.clear();
		m_service->stop();

		if (m_acceptorThread != nullptr && m_acceptorThread->joinable())
			m_acceptorThread->join();
	}

	if (m_receiveSocketPtr != nullptr) {
		if (m_receiveSocketPtr->is_open()) {
			m_receiveSocketPtr->close();
		}
	}
}