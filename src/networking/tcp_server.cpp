#include "networking/tcp_server.hpp"

#include "networking/tcp_session.hpp"

#include <boost/asio/post.hpp>
#include <utility>

namespace kvcache::networking {

TcpServer::TcpServer(
    boost::asio::io_context& io_context,
    unsigned short port,
    server::RequestDispatcher& dispatcher,
    concurrency::ThreadPool& worker_pool
)
    : io_context_(io_context),
      acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      dispatcher_(dispatcher),
      worker_pool_(worker_pool) {}

void TcpServer::Start() {
    if (running_) {
        return;
    }
    running_ = true;
    StartAcceptLoop();
}

void TcpServer::Stop() {
    if (!running_) {
        return;
    }
    running_ = false;

    boost::asio::post(io_context_, [this]() {
        if (!acceptor_.is_open()) {
            return;
        }
        boost::system::error_code ignored_error;
        acceptor_.cancel(ignored_error);
        acceptor_.close(ignored_error);
    });
}

void TcpServer::StartAcceptLoop() {
    if (!running_ || !acceptor_.is_open()) {
        return;
    }
    acceptor_.async_accept([this](boost::system::error_code error, boost::asio::ip::tcp::socket socket) {
        OnAccept(error, std::move(socket));
    });
}

void TcpServer::OnAccept(boost::system::error_code error, boost::asio::ip::tcp::socket socket) {
    if (!running_) {
        return;
    }

    if (!error) {
        auto session = std::make_shared<TcpSession>(std::move(socket), dispatcher_, worker_pool_);
        session->Start();
    } else if (error == boost::asio::error::operation_aborted) {
        return;
    }

    if (running_) {
        StartAcceptLoop();
    }
}

}  // namespace kvcache::networking
