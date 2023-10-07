#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <functional>
#include <string>
#include <map>
#include <tuple>
#include <vector>
#include <thread>
#include <atomic>

#include "Types.hpp"


class RestServer {
public:
    RestServer(uint32_t port);
    ~RestServer();
    void addGetHandler(const std::string& path, std::function<Response(Request)> handler);
    void addPostHandler(const std::string& path, std::function<Response(Request)> handler);
    void start();
    void stop();


private:
    uint32_t port_;
    net::io_context ioc_;
    std::map<std::string, std::function<Response(Request)>> getHandlers_;
    std::map<std::string, std::function<Response(Request)>> postHandlers_;


};
