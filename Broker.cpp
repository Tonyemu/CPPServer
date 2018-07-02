#include "Broker.h"
#include <iostream>
#include "Msg/Request.pb.h"
#include "Msg/RequestHeader.pb.h"
#include "Msg/Response.pb.h"
#include "Msg/ResponseHeader.pb.h"
using namespace boost::asio;
using namespace std;
using namespace com::tonyemu::proto;


TCPServer::TCPServer(const std::string addr, int port) : m_acceptor(m_context, endpoint_b(address_b::from_string(addr),
	port)), daemon(m_context)
{

}

TCPServer::~TCPServer()
{
	if (!threads.empty())
		assert("please call the stop before ~TCPServer");
}
void TCPServer::start(size_t run_thread_count)
{
	for (int i = 0; i < run_thread_count; ++i)
	{
		threads.push_back(new thread([this]{
			m_context.run();
			cout << "run exit" << endl;
		}));
	}
}

void TCPServer::stop()
{
	m_acceptor.close();
	m_context.stop();
	for (std::vector<std::thread*>::iterator iter = threads.begin();
		iter != threads.end(); ++iter)
	{
		(*iter)->join();
		delete *iter;
	}

	threads.clear();
}

KeepLiveServer::KeepLiveServer(const std::string& addr, int port, long time_out, int times) : TCPServer(addr, port),
m_timer(
m_context,
boost::posix_time::
milliseconds(time_out)),
m_strand(m_context),
m_time_out(time_out),
m_times(times)
{
}

void KeepLiveServer::start(size_t run_thread_count)
{
	TCPServer::start(run_thread_count);
	start_timer();
	start_accept();
}

void KeepLiveServer::stop()
{
	TCPServer::stop();
}

void KeepLiveServer::start_accept()
{
	shared_socket ss(new socket_b(m_context));
	m_acceptor.async_accept(*ss, m_strand.wrap([this, ss](const boost::system::error_code& error_code)
	{
		if (error_code)return;
		m_counter[ss] = 0;
		start_accept();
		read(ss);
	}));
}

void KeepLiveServer::read(shared_socket ss)
{
	shared_buff buff(new char[5]);
	async_read(*ss, buffer(buff.get(), 5), m_strand.wrap([this, ss, buff](const boost::system::error_code error_code, std::size_t bytes_transferred)
	{
		if (error_code)
		{
			m_counter.erase(ss);
			return;
		}
		write(ss, buff, bytes_transferred);
		read(ss);
	}));
}

void KeepLiveServer::write(shared_socket ss, shared_buff buff, size_t buff_size)
{
	ss->async_send(buffer(buff.get(), buff_size), m_strand.wrap([this, ss](const boost::system::error_code error_code, std::size_t bytes_transferred)
	{
		if (!error_code){
			m_counter[ss] = 0;
			return;
		}
		m_counter.erase(ss);
	}));
}

void KeepLiveServer::start_timer()
{
	m_timer.async_wait(m_strand.wrap([this](const boost::system::error_code error_code)
	{
		vector<shared_socket> timeout_sockets;
		for (auto iter = m_counter.begin(); iter != m_counter.end(); ++iter)
		{
			iter->second++;
			if (iter->second >= m_times)timeout_sockets.push_back(iter->first);
		}
		m_timer.expires_from_now(boost::posix_time::milliseconds(m_time_out));
		start_timer();
		if (onLost && !timeout_sockets.empty())
			onLost(timeout_sockets);
	}));
}



KeepLiveClient::KeepLiveClient(shared_socket socket, long time_out) : m_timer(socket->get_io_context()),
m_timeout(socket->get_io_context()),
m_time_out(time_out), isStarted(false), m_socket(socket)

{

}

void KeepLiveClient::start(const string& addr, int port)
{
	start_connect(addr, port);
}

void KeepLiveClient::stop()
{
	m_timer.cancel();
	m_timeout.cancel();
	m_socket->close();
}

void KeepLiveClient::start_with_timeout()
{
	m_timeout.cancel();
	m_timer.expires_from_now(boost::posix_time::milliseconds(m_time_out));
	m_timer.async_wait([this](boost::system::error_code error)
	{
		if (error)return;
		start_send();
	});
}

void KeepLiveClient::start_connect(const string& addr, int port)
{
	m_socket->async_connect(endpoint_b(address_b::from_string(addr), port), [this](boost::system::error_code error){
		if (error)return;
		start_send();
	});
}

void KeepLiveClient::start_send()
{
	m_timeout.expires_from_now(boost::posix_time::milliseconds(m_time_out));
	m_timeout.async_wait([this](boost::system::error_code error)
	{
		if (error)return;
		m_socket->close();
		cout << "client lost link" << endl;
	});
	shared_buff buff(new char[5]);
	memcpy(buff.get(), "hello", 5);
	m_socket->async_send(buffer(buff.get(), 5), [this](boost::system::error_code error_code, size_t)
	{
		if (error_code)return;
		start_read();
	});
}

void KeepLiveClient::start_read()
{
	shared_buff buff(new char[5]);
	async_read(*m_socket.get(), buffer(buff.get(), 5), [buff, this](boost::system::error_code error_code, size_t){
		if (error_code)
			return;
		start_with_timeout();
		cout << "hb" << endl;
	});
}


Broker::Broker(const std::string& addr, int port, const std::string& toWorker, int toWorkerPort)
	: TCPServer(addr, port), m_acceptor_inner(m_context), m_strand(m_context), playload_index(0), msgid(0)
{
}

Broker::~Broker()
{

}

void Broker::start(size_t run_thread_count)
{
	TCPServer::start(run_thread_count);
	start_inner_acceptor();
	start_acceptor();
}


void Broker::start_inner_acceptor()
{
	shared_socket ss(new socket_b(m_context));
	m_acceptor_inner.async_accept(*ss, [this, ss](const boost::system::error_code& error_code){
		if (error_code)
			return;
		inner_read_request_size(ss);
		start_inner_acceptor();
	});
}

void Broker::start_acceptor()
{
	shared_socket ss(new socket_b(m_context));
	m_acceptor.async_accept(*ss, [this, ss](const boost::system::error_code& error_code){
		if (error_code)
			return;
		m_clients[ss->local_endpoint()] = ss;
		read_request_size(ss);
		start_acceptor();
	});
}

void Broker::inner_read_request_size(shared_socket ss)
{
	size_t head_size = 10;
	shared_buff buff(new char[head_size]);
	async_read(*ss, buffer(buff.get(), head_size), [ss, this, buff](const boost::system::error_code error_code, std::size_t bytes_transferred)
	{
		if (error_code)
		{
			m_strand.wrap(std::bind(&Broker::closeWorker, this, std::placeholders::_1))(ss);
			return;
		}
		Request req;
		req.ParsePartialFromArray(buff.get(), bytes_transferred);
		m_workers[ss->remote_endpoint()] = ss;
		m_vecWorker.push_back(ss);
		inner_read_request_header(ss, req.bodysize());
	});
}

void Broker::read_request_size(shared_socket ss)
{
	size_t head_size = 10;
	shared_buff buff(new char[head_size]);
	async_read(*ss, buffer(buff.get(), head_size), [ss, this, buff](const boost::system::error_code error_code, std::size_t bytes_transferred)
	{
		if (error_code)
		{
			m_strand.wrap(std::bind(&Broker::closeClient, this, std::placeholders::_1))(ss);
			return;
		}
		Request req;
		req.ParsePartialFromArray(buff.get(), bytes_transferred);
		//reader request header;
		read_request_header(ss, req);
	});
}

void Broker::inner_read_request_header(shared_socket ss, size_t body_size)
{
	shared_buff buff(new char[body_size]);
	async_read(*ss, buffer(buff.get(), body_size), m_strand.wrap([ss, this, buff](const boost::system::error_code error_code, std::size_t bytes_transferred)
	{
		if (error_code)
		{
			closeWorker(ss);
			return;
		}
		onWorker(buff, bytes_transferred);
	}));
}

void Broker::read_request_header(shared_socket ss, const Request& req)
{
	std::shared_ptr<char> buff(new char[req.headersize()]);
	async_read(*ss, buffer(buff.get(), req.headersize()), m_strand.wrap([ss, this, buff](const boost::system::error_code error_code, std::size_t bytes_transferred)
	{
		if (error_code || bytes_transferred == 0)
		{
			closeClient(ss);
			return;
		}
		onRouter(buff, bytes_transferred);
	}));
}

void Broker::inner_read_request_body(shared_socket ss, const Request& req)
{
	std::shared_ptr<char> buff(new char[req.bodysize()]);
	async_read(*ss, buffer(buff.get(), req.bodysize()), m_strand.wrap([ss, this, buff](const boost::system::error_code error_code, std::size_t bytes_transferred)
	{
		if (error_code || bytes_transferred == 0)
		{
			closeClient(ss);
			return;
		}
		onRouter(buff, bytes_transferred);
	}));
}


void Broker::read_request_body()
{
}

void Broker::onRouter(shared_buff buff, size_t size)
{
	if (m_vecWorker.empty())
		m_mailbox.push(SendInfo(nullptr, buff, size));
	shared_socket ss = m_vecWorker[onPlayload()];
	MessageHeaderInner innerHeader;
	innerHeader.set_prefix(0xff);
	innerHeader.set_bodysize(size);
	innerHeader.set_innerid(msgid++);
	size_t inner_buff_size = size + innerHeader.ByteSize();
	shared_buff innerbuff(new char[inner_buff_size]);

	ss->async_send(buffer(buff.get(), 10), [this, buff, ss, size](const boost::system::error_code error_code, std::size_t bytes_transferred)
	{
		if (error_code)
			m_mailbox.push(SendInfo(ss, buff, size));
	});

}

void Broker::onWorker(shared_buff, size_t size)
{

}

void Broker::closeWorker(shared_socket ss)
{
	m_workers.erase(ss->remote_endpoint());
	std::vector<shared_socket>::iterator found = m_vecWorker.begin();
	for (int i = 0; i < m_vecWorker.size(); ++i)
	{
		if (*found == ss)break;
		++found;
	}
	if (found != m_vecWorker.end())
		m_vecWorker.erase(found);
	ss->close();
}

void Broker::closeClient(shared_socket ss)
{
	m_clients.erase(ss->local_endpoint());
	ss->close();
}

int Broker::onPlayload()
{
	int index = playload_index++ %m_vecWorker.size();
	return index;
}

void Broker::stop()
{
	TCPServer::stop();
	m_acceptor_inner.close();
}


Worker::Worker(const std::string& addr, int port)
	: m_broker_addr(addr), m_broker_port(port), m_socket(m_context)
{
}

void Worker::connect()
{
	m_socket.async_connect(endpoint_b(address_b::from_string("127.0.0.1"), 9003), [this](boost::system::error_code error)
	{
		if (error)return;//链接broker失败，需要重新连接
		start_read_header();
	});
}

void Worker::start_read_header()
{
	size_t head_size = 10;
	shared_buff buff(new char[head_size]);
	async_read(m_socket, buffer(buff.get(), 1), [](const boost::system::error_code error_code, std::size_t bytes_transferred){
		if (error_code)return;//读取失败，可能是连接出问题

	});
}
