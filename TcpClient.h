#pragma once
#include <boost/asio.hpp>


typedef boost::asio::io_context context_b;
typedef boost::asio::ip::tcp::socket socket_b;
typedef boost::asio::ip::tcp::endpoint endpoint_t;
typedef boost::asio::ip::address address_t;
class TcpClient
{
	context_b m_context;
	socket_b socket;
	context_b::work work;
public:
	TcpClient();
	void connect(std::function<void(boost::system::error_code)> callback);
	void send();
};
