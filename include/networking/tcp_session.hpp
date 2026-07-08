#pragma once

#include "concurrency/thread_pool.hpp"
#include "server/request_dispatcher.hpp"

#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/error_code.hpp>

#include <deque>
#include <memory>
#include <string>

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
    struct PendingWrite {
        std::string payload;
        bool close_after_write{false};
    };

    void StartReadLoop();
    void OnReadComplete(const boost::system::error_code& error, std::size_t bytes_transferred);
    void EnqueueWrite(std::string payload, bool close_after_write);
    void StartWriteLoop();
    void OnWriteComplete(const boost::system::error_code& error);
    void StopOnStrand();

    boost::asio::ip::tcp::socket socket_;
    boost::asio::strand<boost::asio::any_io_executor> strand_;
    boost::asio::streambuf read_buffer_;
    server::RequestDispatcher& dispatcher_;
    concurrency::ThreadPool& worker_pool_;
    std::deque<PendingWrite> pending_writes_;
    bool writing_{false};
    bool stopped_{false};
};

}  // namespace kvcache::networking
