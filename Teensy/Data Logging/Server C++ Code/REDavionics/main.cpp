#include <iostream>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

using boost::asio::ip::udp;
using json = nlohmann::json;

void processJson(const std::string& jsonString) {
    try {
        json j = json::parse(jsonString);
        for (auto& sensor : j["sensors"]) {
            int type = sensor["type"];
            // Process according to sensor type, for example:
            if (type == 0) { // THERMOCOUPLE
                auto temperatures = sensor["temperatures"];
                for (double temp : temperatures) {
                    std::cout << "Temperature: " << temp << std::endl;
                }
            }
            // Add handling for other sensor types here...
        }
    }
    catch (const json::exception& e) {
        std::cerr << "JSON Exception: " << e.what() << std::endl;
    }
}

int main() {
    try {
        boost::asio::io_service io_service;
        udp::socket socket(io_service, udp::endpoint(udp::v4(), 1234));

        while (true) {
            udp::endpoint sender_endpoint;
            char recv_buffer[2048];
            size_t len = socket.receive_from(boost::asio::buffer(recv_buffer), sender_endpoint);
            std::string jsonString(recv_buffer, len);
            processJson(jsonString);
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
