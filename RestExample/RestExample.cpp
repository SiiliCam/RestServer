
#include "RestServer.hpp"
#include "RestSender.hpp"
#include "Logger.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

int main() {
	RestServer server(3000);
	server.addPostHandler("/test", [](Request req) -> Response {
		boost::property_tree::ptree pt;
		std::istringstream is(req.body());
		boost::property_tree::read_json(is, pt);
		Logger::log_info("SERVER: ", is.str());

		const auto& value = pt.get_optional<int>("value");
		if (value.has_value()) {
			Logger::log_info("SERVER: value:", value.value());
		}
		return Response{ "OK" , 200 };
	});

	std::thread serverThread([&server]() {
		server.start();
		});

	auto serverLambda = []() {
		RestSender sender;
		boost::json::object json_data;
		
		for (int i = 0;;i++){
			json_data["value"] = i;
			auto data = sender.post("localhost", 3000, json_data, "/test");
			Logger::log_info("SENDER: ", data);
		}
		};
	std::vector<std::thread> threads;
	for (int i = 0; i < 30; i++) {

		threads.push_back(std::thread { serverLambda });
	}

	
	serverThread.join();
	for (auto& thread : threads) {
		thread.join();
	}
}
