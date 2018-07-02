#include "TcpClient.h"
#include "MessageHeader.pb.h"
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace com::tonyemu::proto;
TcpClient::TcpClient(): socket(m_context), work(m_context)
{
	std::thread([this]{m_context.run(); }).detach();
}

void TcpClient::connect(std::function<void(boost::system::error_code)> callback)
{
	socket.async_connect(endpoint_t(address_t::from_string("127.0.0.1"), 9003), callback);
	// socket.connect(endpoint_t(address_t::from_string("127.0.0.1"), 9001));
	// MessageHeader mh;
	// mh.set_prefix(0xff);
	// mh.set_bodysize(3);
	// size_t buffSize = mh.ByteSize() + 3;
	// char* buff = new char[buffSize];
	// mh.SerializePartialToArray(buff, buffSize);
 //
	// socket.send(buffer(buff, buffSize));
	// socket.close();
	
}

void TcpClient::send()
{
	 MessageHeader mh;
	 mh.set_prefix(0xff);
	 mh.set_bodysize(4);
	 size_t buffSize = mh.ByteSize() + 4;
	 char* buff = new char[buffSize];
	 mh.SerializePartialToArray(buff, buffSize);
	 *(int*)(buff + 10) = 10;
	 socket.send(buffer(buff, buffSize));
}
