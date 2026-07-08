#include "networking/tcp_session.hpp"

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>

#include <istream>
#include <utility>

namespace kvcache::networking {

TcpSession::TcpSession(
    boost::asio::ip::tcp::socket socket,
    server::RequestDispatcher& dispatcher,
    concurrency::ThreadPool& worker_pool
)
    : socket_(std::move(socket)),
      strand_(boost::asio::make_strand(socket_.get_executor())),
      dispatcher_(dispatcher),
      worker_pool_(worker_pool) {}

void TcpSession::Start() {
    auto self = shared_from_this();
    boost::asio::dispatch(strand_, [self]() { self->StartReadLoop(); });
}

void TcpSession::Stop() {
    auto self = shared_from_this();
    boost::asio::dispatch(strand_, [self]() { self->StopOnStrand(); });
}

void TcpSession::StartReadLoop() {
    if (stopped_) {
        return;
    }

    auto self = shared_from_this();
    boost::asio::async_read_until(
        socket_,
        read_buffer_,
        '\n',
        boost::asio::bind_executor(
            strand_,
            [self](const boost::system::error_code& error, std::size_t bytes_transferred) {
                self->OnReadComplete(error, bytes_transferred);
            }
        )
    );
}

void TcpSession::OnReadComplete(const boost::system::error_code& error, std::size_t) {
    if (error) {
        StopOnStrand();
        return;
    }

    std::istream input_stream(&read_buffer_);
    std::string line;
    std::getline(input_stream, line);
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    auto self = shared_from_this();
    const bool submitted = worker_pool_.Submit([self, line = std::move(line)]() mutable {
        const auto dispatch_result = self->dispatcher_.HandleLine(line);
        boost::asio::post(self->strand_, [self, result = std::move(dispatch_result)]() mutable {
            if (!result.response_line.empty()) {
                self->EnqueueWrite(result.response_line + "\n", result.close_session);
                return;
            }
            if (result.close_session) {
                self->StopOnStrand();
                return;
            }
            self->StartReadLoop();
        });
    });

    if (!submitted) {
        StopOnStrand();
    }
}

void TcpSession::EnqueueWrite(std::string payload, bool close_after_write) {
    if (stopped_) {
        return;
    }

    pending_writes_.push_back({std::move(payload), close_after_write});
    if (writing_) {
        return;
    }
    writing_ = true;
    StartWriteLoop();
}

void TcpSession::StartWriteLoop() {
    if (stopped_ || pending_writes_.empty()) {
        writing_ = false;
        return;
    }

    auto self = shared_from_this();
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(pending_writes_.front().payload),
        boost::asio::bind_executor(
            strand_,
            [self](const boost::system::error_code& error, std::size_t /*bytes_transferred*/) {
                self->OnWriteComplete(error);
            }
        )
    );
}

void TcpSession::OnWriteComplete(const boost::system::error_code& error) {
    if (error) {
        StopOnStrand();
        return;
    }

    const bool close_after_write = pending_writes_.front().close_after_write;
    pending_writes_.pop_front();
    if (close_after_write) {
        StopOnStrand();
        return;
    }

    if (!pending_writes_.empty()) {
        StartWriteLoop();
        return;
    }

    writing_ = false;
    StartReadLoop();
}

void TcpSession::StopOnStrand() {
    if (stopped_) {
        return;
    }
    stopped_ = true;
    writing_ = false;
    pending_writes_.clear();

    boost::system::error_code ignored_error;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_error);
    socket_.close(ignored_error);
}

}  // namespace kvcache::networking
