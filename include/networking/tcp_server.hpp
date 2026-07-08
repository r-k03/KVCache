#pragma once

#include "concurrency/thread_pool.hpp"
#include "server/request_dispatcher.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

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
    void StartAcceptLoop();
    void OnAccept(boost::system::error_code error, boost::asio::ip::tcp::socket socket);

    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    server::RequestDispatcher& dispatcher_;
    concurrency::ThreadPool& worker_pool_;
    bool running_{false};
};

}  // namespace kvcache::networking
