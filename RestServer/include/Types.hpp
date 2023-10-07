#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;


using Request = http::request<http::string_body>;
using Response = std::tuple<std::string, uint32_t>;

using CallBackFunction = std::function<Response(Request)>;