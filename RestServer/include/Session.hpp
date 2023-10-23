#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <map>
#include <chrono>

#include "Types.hpp"

class Session : public std::enable_shared_from_this<Session> {
public:
	Session(tcp::socket socket,
		const std::map<std::string, std::function<Response(Request)>>& getHandlers,
		const std::map<std::string, std::function<Response(Request)>>& postHandlers);

	void start();

private:
	tcp::socket socket_;
	beast::flat_buffer buffer_;
	Request req_;
	std::map<std::string, CallBackFunction> getHandlers_;
	std::map<std::string, CallBackFunction> postHandlers_;
	http::response<http::string_body> res_;

	void readRequest();

	void handleRequest();
	void sendResponse(const Response& response);

};
