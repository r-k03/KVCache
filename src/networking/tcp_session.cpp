#include "networking/tcp_session.hpp"

namespace kvcache::networking {

TcpSession::TcpSession(
    boost::asio::ip::tcp::socket socket,
    server::RequestDispatcher& dispatcher,
    concurrency::ThreadPool& worker_pool
)
    : socket_(std::move(socket)), dispatcher_(dispatcher), worker_pool_(worker_pool) {}

void TcpSession::Start() {
    (void)dispatcher_;
    (void)worker_pool_;
}

void TcpSession::Stop() {
    boost::system::error_code ignored_error;
    socket_.close(ignored_error);
}

}  // namespace kvcache::networking
