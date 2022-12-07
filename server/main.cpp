#include <regex>
#include "Server.h"

int main(int argc, char **argv) {
    // Check if the number of arguments is correct
    if (argc > 3) {
        std::cerr << "Usage (default IP, default PORT): " << argv[0] << std::endl;
        std::cerr << "Usage (default IP, specified PORT): " << argv[0] << " <port>" << std::endl;
        std::cerr << "Usage (specified IP, specified PORT: " << argv[0] << " <ip> <port>" << std::endl;
        std::cerr << "Default IP: INADDR_ANY constant" << std::endl;
        std::cerr << "Default PORT: 0 (random)" << std::endl;
        return EXIT_FAILURE;
    }

    std::string ip;
    uint16_t port = 0;
    std::string port_str;
    std::regex ip_regex(R"((\d{1,3}\.){3}\d{1,3})"); // Regex for IP address validation (IPv4)
    std::regex port_regex(R"(\d{1,5})"); // Regex for port validation

    if (argc == 3) {
        ip = argv[1];
        port_str = argv[2];
    } else if (argc == 2)
        port_str = argv[1];

    if (argc != 1) {
        // Check if the port is valid
        if (!std::regex_match(port_str, port_regex)) {
            std::cerr << "Invalid port" << std::endl;
            return EXIT_FAILURE;
        }
        port = std::stoi(port_str);
        if ((port < MIN_PORT || port > MAX_PORT) && port != 0) {
            std::cerr << "Invalid port" << std::endl;
            return EXIT_FAILURE;
        }

        // Check if the IP is valid
        if (argc == 3) {
            if (!std::regex_match(ip, ip_regex)) {
                std::cerr << "Invalid IP" << std::endl;
                return EXIT_FAILURE;
            }

            // Parse the IP by . and check if it is valid
            std::vector<std::string> ip_parts;
            std::string part;
            std::istringstream iss(ip);
            while (std::getline(iss, part, '.'))
                ip_parts.push_back(part);
            for (auto &i: ip_parts) {
                if (std::stoi(i) > 255) {
                    std::cerr << "Invalid IP" << std::endl;
                    return EXIT_FAILURE;
                }
            }
        }
    }

    // Create the server and run it
    std::unique_ptr<Server> server = std::make_unique<Server>(ip, port);
    server->run();

    return EXIT_SUCCESS;
}
