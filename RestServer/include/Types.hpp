#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sstream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;


using Request = http::request<http::string_body>;
class Response {
public:
    enum class DataType { STRING, JSON_TREE };

    Response(const std::string& data, int statusCode)
        : dataStr(data), statusCode(statusCode), type(DataType::STRING) {}

    Response(const boost::property_tree::ptree& data, int statusCode)
        : dataTree(data), statusCode(statusCode), type(DataType::JSON_TREE) {}

    std::string serialize() const {
        if (type == DataType::STRING) {
            return dataStr;
        }
        else {
            std::ostringstream oss;
            boost::property_tree::write_json(oss, dataTree);
            return oss.str();
        }
    }

    int getStatusCode() const { return statusCode; }

    // Extraction operator for structured bindings
    template <std::size_t I>
    auto get() const {
        if constexpr (I == 0) return serialize();
        if constexpr (I == 1) return getStatusCode();
    }

private:
    std::string dataStr;
    boost::property_tree::ptree dataTree;
    int statusCode;
    DataType type;
};

// Overload for std::tuple_size to let the compiler know how many elements can be extracted
namespace std {
    template <>
    struct tuple_size<Response> : std::integral_constant<std::size_t, 2> {};
} // namespace std

// Overload for std::tuple_element to let the compiler know the types of elements that can be extracted
namespace std {
    template <std::size_t I>
    struct tuple_element<I, Response> {
        using type = decltype(std::declval<Response>().get<I>());
    };
} // namespace std

using CallBackFunction = std::function<Response(Request)>;