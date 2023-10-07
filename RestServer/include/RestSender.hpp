#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>

namespace http = boost::beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class RestSender {
public:
    RestSender();
    ~RestSender();

    std::string get(const std::string& host, uint16_t port = 80, const std::string& path = "/");
    std::string post(const std::string& host, uint16_t port, const boost::json::object& data, const std::string& path = "/");

private:
    net::io_context ioc_;
    tcp::resolver resolver_;
};
