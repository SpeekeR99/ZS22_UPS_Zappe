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

    init_commands_map();
}

Server::~Server() {
    close(server_socket);
}

void Server::init_commands_map() {
    commands["HELLO"] = &Server::handshake;
    commands["LOGIN"] = &Server::login;
}

void Server::clear_char_buffer() {
    for (char & i : char_buffer)
        i = 0;
}

void Server::read_message(int fd) {
    read(fd, char_buffer, BUFFER_LEN);
    buffer = std::string(char_buffer);
    clear_char_buffer();
}

void Server::send_message(int fd, const std::string &message) {
    send(fd, message.c_str(), message.length(), 0);
}

void Server::recv_message(int fd, int a2read) {
    recv(fd, char_buffer, a2read, 0);
    buffer = std::string(char_buffer);
    clear_char_buffer();
}

std::vector<std::string>  Server::tokenize_buffer(char delim) const {
    std::stringstream ss(buffer);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, delim))
        tokens.push_back(token);

    return tokens;
}

std::shared_ptr<Player> Server::get_player_by_fd(int fd) {
    std::shared_ptr<Player> player;
    for (auto & i : players)
        if (i->socket == fd) player = i;
    return player;
}

bool Server::is_player_logged_in(int fd) {
    auto player = get_player_by_fd(fd);
    return player->handshake && player->is_logged_in;
}

bool Server::is_name_taken(const std::string &name) {
    for (auto & i : players)
        if (i->name == name) return true;
    return false;
}

void Server::handle_incoming_message(int fd) {
    auto player = get_player_by_fd(fd);

    std::cout << "Received message from client " << fd << " (" << player->name << "): " << buffer << std::endl;

    auto tokens = tokenize_buffer(COMMAND_DELIMITER);
    std::string cmd = tokens[0];
    tokens.erase(tokens.begin());
    cmd.erase(std::remove_if(cmd.begin(), cmd.end(), ::isspace), cmd.end());

    if (commands.count(cmd))
        (this->*commands[cmd])(fd, tokens);
    else
        std::cerr << "ERROR: Unknown command: " << cmd << std::endl;
}

void Server::handshake(int fd, const std::vector<std::string> &params) {
    send_message(fd, "HELLO\n");
    get_player_by_fd(fd)->handshake = true;
}

void Server::login(int fd, const std::vector<std::string> &params) {
    if (params.size() != 1) {
        std::cerr << "ERROR: Invalid number of parameters for LOGIN command" << std::endl;
        send_message(fd, "LOGIN|ERR|Invalid number of parameters\n");
        return;
    }
    auto name = params[0];
    name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());

    if(is_name_taken(params[0])) {
        std::cerr << "ERROR: Nickname " << name << " is already taken" << std::endl;
        send_message(fd, "LOGIN|ERR|Nickname is already taken\n");
        return;
    }

    auto player = get_player_by_fd(fd);
    player->name = name;
    player->is_logged_in = true;

    std::cout << "Player " << player->name << " logged in" << std::endl;

    send_message(fd, "LOGIN|OK\n");
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
        clear_char_buffer();

        // Wait for activity on one of the sockets
        if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0) {
            std::cerr << "ERR: select" << std::endl;
            break;
        }

        // Check if there is activity on stdin (server commands)
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            read_message(STDIN_FILENO);
            if (buffer == "exit\n")
                break;
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
                        players.push_back(std::make_shared<Player>(client_socket));
                        std::cout << "New client connected: " << client_socket << std::endl;

                    // It is a message from a client
                    } else {
                        int a2read;
                        ioctl(fd, FIONREAD, &a2read);

                        // Client sent some bytes
                        if (a2read > 0) {
                            recv_message(fd, a2read);
                            handle_incoming_message(fd);

                        // Client disconnected
                        } else {
                            close(fd);
                            FD_CLR(fd, &client_socks);
                            std::cout << "Client " << fd << " disconnected" << std::endl;
                            players.erase(std::remove_if(players.begin(), players.end(),
                                                         [fd](std::shared_ptr<Player> player) {
                                                             return player->socket == fd;
                                                         }), players.end());
                        }
                    }
                }
            }
        }
    }

    std::cout << "Server shutting down..." << std::endl;
}
