#include "command/command_executor.hpp"
#include "command/command_table.hpp"
#include "command/line_parser.hpp"
#include "concurrency/thread_pool.hpp"
#include "networking/tcp_server.hpp"
#include "server/request_dispatcher.hpp"
#include "storage/thread_safe_kv_store.hpp"

#include <boost/asio/io_context.hpp>

#include <algorithm>
#include <cstddef>
#include <exception>
#include <iostream>
#include <thread>

namespace {

constexpr unsigned short kDefaultPort = 6380;

std::size_t DefaultWorkerCount() {
    const unsigned int hw_threads = std::thread::hardware_concurrency();
    return std::max<std::size_t>(2, hw_threads == 0 ? 2 : hw_threads);
}

}  // namespace

int main() {
    try {
        const kvcache::command::CommandTable command_table;
        kvcache::storage::ThreadSafeKVStore store;
        kvcache::command::CommandExecutor command_executor(store);
        const kvcache::command::LineParser parser(command_table);
        kvcache::server::RequestDispatcher dispatcher(parser, command_executor);

        boost::asio::io_context io_context;
        kvcache::concurrency::ThreadPool worker_pool(DefaultWorkerCount());
        worker_pool.Start();

        kvcache::networking::TcpServer server(io_context, kDefaultPort, dispatcher, worker_pool);
        server.Start();

        std::cout << "KVCache TCP server listening on port " << kDefaultPort << "\n";
        io_context.run();

        server.Stop();
        worker_pool.Stop();
    } catch (const std::exception& error) {
        std::cerr << "Server failed: " << error.what() << "\n";
        return 1;
    }

    return 0;
}
