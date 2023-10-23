#include <memory>
#include <chrono>
#include <iostream>
#include <boost/json.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "Session.hpp"
#include "Logger.hpp"

Session::Session(tcp::socket socket,
	const std::map<std::string, std::function<Response(Request)>>& getHandlers,
	const std::map<std::string, std::function<Response(Request)>>& postHandlers)
	: socket_(std::move(socket)), getHandlers_(getHandlers), postHandlers_(postHandlers) {
}

void Session::start() {
	readRequest();
}

void Session::readRequest() {
	http::async_read(socket_, buffer_, req_,
		[self = shared_from_this()](beast::error_code ec, std::size_t) {
            if (ec) {
                Logger::log_error("Error reading request:", ec.message());
            }

			if (!ec) {
				self->handleRequest();
			}
		});
}

// HandleRequest now sends a Response object to sendResponse
void Session::handleRequest() {
    std::string path = req_.target();
    if (req_.method() == http::verb::get) {
        auto it = getHandlers_.find(path);
        if (it != getHandlers_.end()) {
            auto response = it->second(req_);
            sendResponse(response);
        }
        else {
            sendResponse(Response("Not Found", 404));
        }
    }
    else if (req_.method() == http::verb::post) {
        auto it = postHandlers_.find(path);
        if (it != postHandlers_.end()) {
            auto response = it->second(req_);
            sendResponse(response);
        }
        else {
            sendResponse(Response("Not Found", 404));
        }
    }
    else {
        sendResponse(Response("Unsupported request method", 405));
    }
}

void Session::sendResponse(const Response& response) {
    auto message = response.serialize();
    auto status = response.getStatusCode();
    auto contentType = response.getDataType();

    res_ = { static_cast<http::status>(status), req_.version() };
    res_.set(http::field::server, BOOST_BEAST_VERSION_STRING);

    if (contentType == Response::DataType::JSON_TREE) {
        // If the content is already JSON, send it directly
        res_.body() = message;
    }
    else {
        // Otherwise, wrap it in a JSON object with "message" as the key
        boost::json::object jsonBody;
        jsonBody["message"] = message;
        res_.body() = boost::json::serialize(jsonBody);
    }

    res_.set(http::field::content_type, "application/json");
    res_.set(http::field::content_length, std::to_string(res_.body().size()));
    res_.prepare_payload();

    boost::system::error_code ec;
    http::async_write(socket_, res_, [self = shared_from_this()](beast::error_code ec, std::size_t) {
        if (ec) {
            Logger::log_error("Error during write:", ec.message());
        }
        self->socket_.shutdown(tcp::socket::shutdown_send, ec);
        auto end = std::chrono::steady_clock::now();
        });
}