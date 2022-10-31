#include "Server.h"

Server::Server(int port) {
    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Set socket options
    int param = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char *) &param, sizeof(int)) == -1)
        std::cerr << "ERR: setsockopt" << std::endl;
    else
        std::cout << "OK: setsockopt" << std::endl;

    // Set server address
    struct sockaddr_in server_address{};
    memset(&server_address, 0, sizeof(struct sockaddr_in));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    if (!bind(server_socket, (struct sockaddr *) &server_address, sizeof(struct sockaddr_in)))
        std::cout << "OK: bind" << std::endl;
    else
        std::cerr << "ERR: bind" << std::endl;

    // Listen for incoming connections
    if (!listen(server_socket, 5))
        std::cout << "OK: listen" << std::endl;
    else
        std::cerr << "ERR: listen" << std::endl;

    std::cout << "Server started on port " << port << std::endl;
}

Server::~Server() {
    close(server_socket);
}

void Server::run() {
    std::cout << "Server running..." << std::endl;

    // Initialize client sockets
    fd_set client_socks, read_fds;
    FD_ZERO(&client_socks);
    FD_SET(server_socket, &client_socks);
    FD_SET(STDIN_FILENO, &client_socks);

    for (;;) {
        // Copy client_socks to read_fds (select() modifies read_fds)
        read_fds = client_socks;

        // Clear char_buffer
        for (char & i : char_buffer)
            i = 0;

        // Wait for activity on one of the sockets
        if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0) {
            std::cerr << "ERR: select" << std::endl;
            break;
        }

        // Check if there is activity on stdin (server commands)
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            read(STDIN_FILENO, char_buffer, BUFFER_LEN);
            if (!strcmp(char_buffer, "exit\n")) {
                break;
            }
        }

        // Check if there is activity on server socket
        else {
            for (int fd = 3; fd < FD_SETSIZE; fd++) {
                if (FD_ISSET(fd, &read_fds)) {

                    // It is a new connection
                    if (fd == server_socket) {
                        int client_socket;
                        socklen_t len_addr;
                        struct sockaddr_in client_address{};
                        client_socket = accept(server_socket, (struct sockaddr *) &client_address, &len_addr);
                        FD_SET(client_socket, &client_socks);
                        std::cout << "New client connected: " << client_socket << std::endl;

                    // It is a message from a client
                    } else {
                        int a2read;
                        ioctl(fd, FIONREAD, &a2read);

                        // Client send some bytes
                        if (a2read > 0) {
                            recv(fd, char_buffer, a2read, 0);
                            std::cout << "Received from client " << fd << ": " << char_buffer << std::endl;
                            // TODO: Handle client messages

                        // Client disconnected
                        } else {
                            close(fd);
                            FD_CLR(fd, &client_socks);
                            std::cout << "Client " << fd << " disconnected" << std::endl;
                        }
                    }
                }
            }
        }
    }

    std::cout << "Server shutting down..." << std::endl;
}
