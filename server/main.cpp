#include "Server.h"

int main(int argc, char **argv) {
    // Check if the number of arguments is correct
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

    // Create the server and run it
    std::unique_ptr<Server> server = std::make_unique<Server>(std::stoi(argv[1]));
    server->run();

    return EXIT_SUCCESS;
}
