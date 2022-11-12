#include "Server.h"

int main() {
    std::unique_ptr<Server> server = std::make_unique<Server>(12345);
    server->run();

    return EXIT_SUCCESS;
}
