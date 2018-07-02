#pragma once
#include <boost/asio.hpp>
#include <queue>

namespace com {
	namespace tonyemu {
		namespace proto {
			class Request;
		}
	}
}

typedef boost::asio::io_context context_b;
typedef context_b::strand strand_b;
typedef context_b::work context_daemon;
typedef boost::asio::ip::tcp::socket socket_b;
typedef boost::asio::ip::address address_b;
typedef boost::asio::ip::tcp::endpoint endpoint_b;
typedef boost::asio::ip::tcp::acceptor acceptor_b;
typedef std::shared_ptr<socket_b> shared_socket;
typedef std::shared_ptr<char> shared_buff;
typedef std::map<endpoint_b, shared_socket> map_sockets;
typedef std::function<void(boost::system::error_code)> accept_handle_type;
typedef std::function<void(boost::system::error_code, size_t)> read_write_handle_type;
class SendInfo
{
public:

	SendInfo(const shared_socket& basic_stream_socket, const shared_buff& buff,size_t buff_size)
		: ss(basic_stream_socket),
		buff(buff), buff_size(buff_size)
	{
	}

	shared_socket ss;
	shared_buff buff;
	size_t buff_size;
};

class TCPServer
{
protected:
	context_b m_context;
	acceptor_b m_acceptor;
	std::vector<std::thread*> threads;
	context_daemon daemon;
public:
	virtual ~TCPServer();
	TCPServer(std::string addr, int port);
	virtual void start(size_t run_thread_count);
	virtual void stop();
};

class KeepLiveServer:public TCPServer
{
protected:
	std::map<shared_socket, int> m_counter;
	boost::asio::deadline_timer m_timer;
	context_b::strand m_strand;
	std::function<void(std::vector<shared_socket>)> onLost;
	void start_accept();
	void read(shared_socket ss);
	void write(shared_socket ss, shared_buff buff, size_t buff_size);
	void start_timer();
	long m_time_out;
	int m_times;
public:

	KeepLiveServer(const std::string& addr, int port, long time_out, int times);
	
	virtual void start(size_t run_thread_count) override;
	virtual void stop() override;

	void setOnlost(const std::function<void(std::vector<shared_socket>&)>& on_lost)
	{
		onLost = on_lost;
	}

};

class KeepLiveClient
{
protected:
	shared_socket m_socket;
	boost::asio::deadline_timer m_timer;
	boost::asio::deadline_timer m_timeout;
	long m_time_out;
	bool isStarted;
	void start_with_timeout();
	void start_connect(const std::string& addr, int port);
	void start_send();
	void start_read();
public:
	long getTimeOut() const
	{
		return m_time_out;
	}

	void setTimeOut(long time_out)
	{
		m_time_out = time_out;
	}

	KeepLiveClient(shared_socket socket, long time_out);
	void start(const std::string& addr, int port);
	void stop();

	//todo:添加onlost
};

class Broker:public TCPServer
{

public:
	Broker(const std::string& addr, int port, const std::string& toWorker, int toWorkerPort);

	virtual ~Broker() override;
	virtual void start(size_t run_thread_count) override;
	virtual void stop() override;
protected:

	acceptor_b m_acceptor_inner;
	map_sockets m_workers;
	std::vector<shared_socket> m_vecWorker;
	strand_b m_strand;
	std::atomic<int> playload_index;
	std::queue<SendInfo> m_mailbox;
	map_sockets m_clients;
	unsigned long msgid;
	void start_inner_acceptor();
	void start_acceptor();
//
	void inner_read_request_size(shared_socket ss);
	void read_request_size(shared_socket ss);
//
 	void inner_read_request_header(shared_socket ss, size_t body_size);
 	void read_request_header(shared_socket ss, const com::tonyemu::proto::Request& req);

	void inner_read_request_body(shared_socket ss, const com::tonyemu::proto::Request& req);
	void read_request_body();

 	void onRouter(shared_buff buff,size_t size);
 	void onWorker(shared_buff buff, size_t size);
 	void closeWorker(shared_socket ss);
	void closeClient(shared_socket ss);
	int onPlayload();
};

class Worker
{
public:
	Worker(const std::string& broker_addr, int port);
	void connect();

protected:
	std::string m_broker_addr;
	int m_broker_port;
	socket_b m_socket;
	context_b m_context;

	void start_read_header();
	
};