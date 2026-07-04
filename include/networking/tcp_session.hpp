#pragma once

#include "concurrency/thread_pool.hpp"
#include "server/request_dispatcher.hpp"

#include <boost/asio/ip/tcp.hpp>

#include <memory>

namespace kvcache::networking {

class TcpSession : public std::enable_shared_from_this<TcpSession> {
public:
    TcpSession(
        boost::asio::ip::tcp::socket socket,
        server::RequestDispatcher& dispatcher,
        concurrency::ThreadPool& worker_pool
    );

    void Start();
    void Stop();

private:
    boost::asio::ip::tcp::socket socket_;
    server::RequestDispatcher& dispatcher_;
    concurrency::ThreadPool& worker_pool_;
};

}  // namespace kvcache::networking
