#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
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
constexpr uint32_t DC_TIMEOUT_SEC = 30;

class Server {
private:
    typedef void (Server::*command) (int, const std::vector<std::string> &);
    typedef std::map<std::string, command> command_map;

    int server_socket;
    fd_set client_socks{};
    char char_buffer[BUFFER_LEN]{};
    std::string buffer;
    std::vector<std::shared_ptr<Player>> players;
    std::vector<std::shared_ptr<Player>> disconnected_players;
    uint32_t game_id;
    std::vector<std::shared_ptr<Game>> games;
    command_map commands;

    void init_commands_map();
    void clear_char_buffer();
    void read_message(int fd);
    void send_message(int fd, const std::string &message);
    void recv_message(int fd, int a2read);
    std::vector<std::string> tokenize_buffer(char delim) const;
    std::shared_ptr<Player> get_player_by_fd(int fd);
    std::shared_ptr<Game> get_game_by_id(uint32_t id);
    bool is_name_taken(const std::string &name);
    bool does_game_exist(uint32_t id);
    void disconnect_player(int fd);
    void player_error_message_inc(int fd);

    void handle_incoming_message(int fd);
    void handshake(int fd, const std::vector<std::string> &params);
    void login(int fd, const std::vector<std::string> &params);
    void logout(int fd, const std::vector<std::string> &params);
    void reconnect(int fd, const std::vector<std::string> &params);
    void create_lobby(int fd, const std::vector<std::string> &params);
    void join_lobby(int fd, const std::vector<std::string> &params);
    void leave_lobby(int fd, const std::vector<std::string> &params);
    void list_lobbies(int fd, const std::vector<std::string> &params);

public:
    explicit Server(int port);
    ~Server();

    void run();
};
