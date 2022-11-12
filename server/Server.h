#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include "game/StateMachine.h"
#include "game/Game.h"
#include "game/Player.h"
#include "game/MyRandom.h"

constexpr size_t BUFFER_LEN = 1024;
constexpr char COMMAND_DELIMITER = '|';

class Server {
private:
    typedef void (Server::*command) (int, const std::vector<std::string> &);
    typedef std::map<std::string, command> command_map;

    int server_socket;
    char char_buffer[BUFFER_LEN]{};
    std::string buffer;
    std::vector<std::shared_ptr<Player>> players;
    std::vector<std::shared_ptr<Game>> games;
    command_map commands;

    void init_commands_map();
    void clear_char_buffer();
    void read_message(int fd);
    void send_message(int fd, const std::string &message);
    void recv_message(int fd, int a2read);
    std::vector<std::string> tokenize_buffer(char delim) const;
    std::shared_ptr<Player> get_player_by_fd(int fd);
    bool is_player_logged_in(int fd);
    bool is_name_taken(const std::string &name);
    void handle_incoming_message(int fd);

    void handshake(int fd, const std::vector<std::string> &params);
    void login(int fd, const std::vector<std::string> &params);

public:
    explicit Server(int port);
    ~Server();

    void run();
};
