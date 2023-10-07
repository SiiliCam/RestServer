#include "RestSender.hpp"

RestSender::RestSender() : resolver_(ioc_) {}

RestSender::~RestSender() {
    ioc_.stop();
}

std::string RestSender::get(const std::string& host, uint16_t port, const std::string& path) {
    // Resolve the host with the given port
    auto results = resolver_.resolve(host, std::to_string(port));

    // Establish a connection
    tcp::socket socket(ioc_);
    net::connect(socket, results.begin(), results.end());

    // Send GET request
    http::request<http::string_body> req(http::verb::get, path, 11);
    http::write(socket, req);

    // Read the response
    boost::beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(socket, buffer, res);

    return res.body();
}

std::string RestSender::post(const std::string& host, uint16_t port, const boost::json::object& data, const std::string& path) {
    // Resolve the host with the given port
    auto results = resolver_.resolve(host, std::to_string(port));

    // Establish a connection
    tcp::socket socket(ioc_);
    net::connect(socket, results.begin(), results.end());

    // Send POST request with JSON data
    http::request<http::string_body> req(http::verb::post, path, 11);
    req.set(http::field::content_type, "application/json");
    req.body() = boost::json::serialize(data);
    req.prepare_payload();
    http::write(socket, req);

    // Read the response
    boost::beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(socket, buffer, res);

    return res.body();
}
