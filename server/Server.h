#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include "game/Game.h"
#include "game/Player.h"

constexpr size_t BUFFER_LEN = 1024;
constexpr char COMMAND_DELIMITER = '|';

class Server {
private:
    int server_socket;
    char char_buffer[BUFFER_LEN]{};
    std::string buffer;
    std::vector<std::shared_ptr<Player>> players;
    std::vector<std::shared_ptr<Game>> games;

    void clear_char_buffer();
    void read_message(int fd);
    void send_message(int fd, const std::string &message);
    void recv_message(int fd, int a2read);
    [[nodiscard]] std::vector<std::string> tokenize_buffer(char delim) const;
    void handle_incoming_message(int fd);

public:
    explicit Server(int port);
    ~Server();

    void run();
};
