#include "Server.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

    std::unique_ptr<Server> server = std::make_unique<Server>(std::stoi(argv[1]));
    server->run();

    return EXIT_SUCCESS;
}
