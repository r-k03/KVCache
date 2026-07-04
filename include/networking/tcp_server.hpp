#pragma once

#include "concurrency/thread_pool.hpp"
#include "server/request_dispatcher.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace kvcache::networking {

class TcpServer {
public:
    TcpServer(
        boost::asio::io_context& io_context,
        unsigned short port,
        server::RequestDispatcher& dispatcher,
        concurrency::ThreadPool& worker_pool
    );

    void Start();
    void Stop();

private:
    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    server::RequestDispatcher& dispatcher_;
    concurrency::ThreadPool& worker_pool_;
};

}  // namespace kvcache::networking
