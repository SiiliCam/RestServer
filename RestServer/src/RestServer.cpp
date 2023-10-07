#include <memory>
#include <iostream>
#include "RestServer.hpp"
#include "Session.hpp"
#include <csignal>
#include "Logger.hpp"

static RestServer* globalServerInstance = nullptr;

RestServer::RestServer(uint32_t port) : port_(port) {

	globalServerInstance = this;
}

RestServer::~RestServer() {
	stop();
}

static void signalHandler(int signum) {
	// You'll need a way to access the RestServer instance or the io_context from here.
	// One way is to make the io_context a static member, or you can use other methods
	// to ensure you have a pointer/reference to the io_context.
	if (globalServerInstance) {
		globalServerInstance->stop();
	}
}

void RestServer::stop() {
	ioc_.stop();
}

void RestServer::addGetHandler(const std::string& path, std::function<Response(Request)> handler) {
	getHandlers_[path] = handler;
}
void RestServer::addPostHandler(const std::string& path, std::function<Response(Request)> handler) {
	postHandlers_[path] = handler;
}
void RestServer::start() {
	std::signal(SIGINT, signalHandler);
	std::signal(SIGTERM, signalHandler);

	// Use a resolver to get the endpoint
	boost::asio::ip::tcp::resolver resolver(ioc_);
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve("0.0.0.0", std::to_string(port_));

	// Set up the acceptor
	tcp::acceptor acceptor(ioc_);
	acceptor.open(endpoint.protocol());
	acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));

	acceptor.bind(endpoint);
	acceptor.listen();

	Logger::log_info("Server started and listening on port", port_);

	// Asynchronous accept loop
	std::function<void()> do_accept;
	do_accept = [&]() {
		acceptor.async_accept([this, &do_accept](boost::system::error_code ec, tcp::socket socket) {

			if (socket.is_open()) {
				socket.set_option(boost::asio::ip::tcp::no_delay(true));
			}

			if (!ec) {
				std::make_shared<Session>(std::move(socket), getHandlers_, postHandlers_)->start();
			}
			else {
				Logger::log_error("Error accepting connection :", ec.message());
			}
			// Continue accepting new connections
			do_accept();
			});
		};

	// Start the asynchronous accept loop
	do_accept();
	auto thread_count = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	for (std::size_t i = 0; i < thread_count; ++i) {
		threads.emplace_back([this] {
			try {
				ioc_.run();
			}
			catch (const std::exception& e) {
				Logger::log_error("Thread exception:", e.what());
			}
			});
	}

	for (auto& thread : threads) {
		thread.join();
	}
}
