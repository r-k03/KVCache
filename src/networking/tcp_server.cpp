#include "networking/tcp_server.hpp"

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
    (void)io_context_;
    (void)dispatcher_;
    (void)worker_pool_;
}

void TcpServer::Stop() {
    boost::system::error_code ignored_error;
    acceptor_.close(ignored_error);
}

}  // namespace kvcache::networking
