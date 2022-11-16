#include "Server.h"

Server::Server(int port) : game_id(0) {
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
    commands["LOGOUT"] = &Server::logout;
    commands["RECONNECT"] = &Server::reconnect;
    commands["CREATE_LOBBY"] = &Server::create_lobby;
    commands["JOIN_LOBBY"] = &Server::join_lobby;
    commands["LEAVE_LOBBY"] = &Server::leave_lobby;
    commands["LIST_LOBBIES"] = &Server::list_lobbies;
    commands["GAME_STATUS"] = &Server::game_status;
    commands["REROLL"] = &Server::reroll;
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

std::shared_ptr<Game> Server::get_game_by_id(uint32_t id) {
    std::shared_ptr<Game> game;
    for (auto & i : games)
        if (i->id == id) game = i;
    return game;
}

bool Server::is_name_taken(const std::string &name) {
    return std::any_of(players.begin(), players.end(), [&name](const std::shared_ptr<Player> &player) {
        return player->name == name;
    }) || std::any_of(disconnected_players.begin(), disconnected_players.end(), [&name](const std::shared_ptr<Player> &player) {
        return player->name == name;
    });
}

bool Server::does_game_exist(uint32_t id) {
    return std::any_of(games.begin(), games.end(), [id](const std::shared_ptr<Game> &game) {
        return game->id == id;
    });
}

void Server::disconnect_player(int fd) {
    std::cout << "Client " << fd << " disconnected" << std::endl;
    close(fd);
    FD_CLR(fd, &client_socks);
    players.erase(std::remove_if(players.begin(), players.end(),
                                 [fd](const std::shared_ptr<Player>& player) {
                                     return player->socket == fd;
                                 }), players.end());
}

void Server::player_error_message_inc(int fd) {
    auto player = get_player_by_fd(fd);
    player->number_of_error_messages++;

    // Disconnect player if he sent too many error messages
    if (player->number_of_error_messages >= 3) {
        std::cout << "Player " << player->name << " has been disconnected for too many error messages" << std::endl;
        send_message(fd, "Too many error messages. Disconnecting...\n");
        disconnect_player(fd);
    }
}

void Server::handle_incoming_message(int fd) {
    auto player = get_player_by_fd(fd);

    std::cout << "Received message from client " << fd << " (" << player->name << "): " << buffer << std::endl;

    auto tokens = tokenize_buffer(COMMAND_DELIMITER);
    std::string cmd = tokens[0];
    tokens.erase(tokens.begin());
    cmd.erase(std::remove_if(cmd.begin(), cmd.end(), ::isspace), cmd.end());

    // Check if command exists
    if (commands.count(cmd)) {
        // Check if player has sent HELLO message and is logged in (or is trying to send HELLO message / login / reconnect)
        if ((player->handshake && player->logged_in) || cmd == "HELLO" || cmd == "LOGIN" || cmd == "RECONNECT")
            (this->*commands[cmd])(fd, tokens);
        else {
            std::cerr << "ERROR: Client " << fd << " (" << player->name << ") has either not yet done handshake or is not logged in" << std::endl;
            send_message(fd, "CMD|ERR|You must handshake and login first\n");
            player_error_message_inc(fd);
        }
    } else {
        std::cerr << "ERROR: Unknown command: " << cmd << std::endl;
        send_message(fd, "CMD|ERR|Unknown command|" + cmd + "\n");
        player_error_message_inc(fd);
    }
}

void Server::handshake(int fd, const std::vector<std::string> &params) {
    // Check if the number of parameters is correct
    if (!params.empty()) {
        std::cerr << "ERROR: Invalid number of parameters for HELLO command" << std::endl;
        send_message(fd, "HELLO|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    send_message(fd, "HELLO\n");
    get_player_by_fd(fd)->handshake = true;
    get_player_by_fd(fd)->number_of_error_messages = 0;
}

void Server::login(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);
    auto name = params[0];
    name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());

    // Check if the number of parameters is correct
    if (params.size() != 1) {
        std::cerr << "ERROR: Invalid number of parameters for LOGIN command" << std::endl;
        send_message(fd, "LOGIN|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player is not already logged in
    if (player->logged_in) {
        std::cerr << "ERROR: Player " << player->name << " is already logged in" << std::endl;
        send_message(fd, "LOGIN|ERR|You are already logged in\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the name is not taken
    if(is_name_taken(name)) {
        std::cerr << "ERROR: Nickname " << name << " is already taken" << std::endl;
        send_message(fd, "LOGIN|ERR|Nickname is already taken\n");
        return;
    }

    player->name = name;
    player->logged_in = true;
    player->number_of_error_messages = 0;

    std::cout << "Player " << player->name << " logged in" << std::endl;
    send_message(fd, "LOGIN|OK\n");
}

void Server::logout(int fd, const std::vector<std::string> &params) {
    // Check if the number of parameters is correct
    if (!params.empty()) {
        std::cerr << "ERROR: Invalid number of parameters for GOODBYE command" << std::endl;
        send_message(fd, "GOODBYE|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    std::cout << "Player " << get_player_by_fd(fd)->name << " logged out" << std::endl;
    send_message(fd, "GOODBYE\n");
    disconnect_player(fd);
}

void Server::reconnect(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);
    auto name = params[0];
    name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());

    // Check if the number of parameters is correct
    if (params.size() != 1) {
        std::cerr << "ERROR: Invalid number of parameters for RECONNECT command" << std::endl;
        send_message(fd, "RECONNECT|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the name exists
    if (!is_name_taken(name)) {
        std::cerr << "ERROR: Player " << name << " does not exist" << std::endl;
        send_message(fd, "RECONNECT|ERR|Name does not exist\n");
        player_error_message_inc(fd);
        return;
    }

    std::shared_ptr<Player> player_reconnecting_to;
    for (auto &i : disconnected_players) {
        if (i->name == name) {
            player_reconnecting_to = i;
            break;
        }
    }

    // Check if the destination player is really disconnected
    if (player_reconnecting_to->logged_in || player_reconnecting_to->state != P_S_DISCONNECTED) {
        std::cerr << "ERROR: Player " << player->name << " is not disconnected" << std::endl;
        send_message(fd, "RECONNECT|ERR|Player is not disconnected\n");
        player_error_message_inc(fd);
        return;
    }

    player->name = name;
    player->logged_in = true;
    player->number_of_error_messages = 0;

    disconnected_players.erase(std::remove_if(disconnected_players.begin(), disconnected_players.end(),
                                 [player_reconnecting_to](const std::shared_ptr<Player>& player) {
                                     return player == player_reconnecting_to;
                                 }), disconnected_players.end());

    std::cout << "Player " << player->name << " reconnected" << std::endl;
    send_message(fd, "RECONNECT|OK\n");
}

void Server::create_lobby(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);

    // Check if the number of parameters is correct
    if (!params.empty()) {
        std::cerr << "ERROR: Invalid number of parameters for CREATE_LOBBY command" << std::endl;
        send_message(fd, "CREATE_LOBBY|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player is in main menu
    if (player->state != P_S_IN_MAIN_MENU) {
        std::cerr << "ERROR: Player " << player->name << " is not in main menu" << std::endl;
        send_message(fd, "CREATE_LOBBY|ERR|You are not in main menu\n");
        player_error_message_inc(fd);
        return;
    }

    // Create a new lobby
    auto game = std::make_shared<Game>(game_id++, player);
    player->game = game;
    games.push_back(game);
    player->number_of_error_messages = 0;

    std::cout << "Player " << player->name << " created a lobby " << game->id << std::endl;
    send_message(fd, "CREATE_LOBBY|OK|" + std::to_string(game->id) + "\n");
}

void Server::join_lobby(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);

    // Check if the number of parameters is correct
    if (params.size() != 1) {
        std::cerr << "ERROR: Invalid number of parameters for JOIN_LOBBY command" << std::endl;
        send_message(fd, "JOIN_LOBBY|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player is in main menu
    if (player->state != P_S_IN_MAIN_MENU) {
        std::cerr << "ERROR: Player " << player->name << " is not in main menu" << std::endl;
        send_message(fd, "CREATE_LOBBY|ERR|You are not in main menu\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the lobby exists
    auto id = std::stoi(params[0]);
    if (!does_game_exist(id)) {
        std::cerr << "ERROR: Lobby " << id << " does not exist" << std::endl;
        send_message(fd, "JOIN_LOBBY|ERR|Lobby does not exist\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the lobby is full
    auto game = get_game_by_id(id);
    if (game->state != G_S_WAITING_FOR_PLAYERS) {
        std::cerr << "ERROR: Lobby " << id << " is full" << std::endl;
        send_message(fd, "JOIN_LOBBY|ERR|Lobby is full\n");
        player_error_message_inc(fd);
        return;
    }

    // Join the lobby
    std::cout << "Player " << player->name << " joined lobby " << game->id << std::endl;

    // Notify both players about their opponent
    auto opponent = game->get_opponent(player);
    send_message(fd, "JOIN_LOBBY|OK|" + opponent->name + "\n");
    send_message(opponent->socket, "JOIN_LOBBY|OK|" + player->name + "\n");

    player->number_of_error_messages = 0;
    game->join_game(player);
    std::cout << "Game " << id << " started" << std::endl;
}

void Server::leave_lobby(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);

    // Check if the number of parameters is correct
    if (!params.empty()) {
        std::cerr << "ERROR: Invalid number of parameters for LEAVE_LOBBY command" << std::endl;
        send_message(fd, "LEAVE_LOBBY|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player is in a lobby
    if (player->state != P_S_IN_LOBBY) {
        std::cerr << "ERROR: Player " << player->name << " is not in a lobby" << std::endl;
        send_message(fd, "LEAVE_LOBBY|ERR|You are not in a lobby\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the game is waiting for players
    if (player->game->state != G_S_WAITING_FOR_PLAYERS) {
        std::cerr << "ERROR: Game " << player->game->id << " is not waiting for players" << std::endl;
        send_message(fd, "LEAVE_LOBBY|ERR|Game is not waiting for players\n");
        player_error_message_inc(fd);
        return;
    }

    // Leave the lobby
    auto game = player->game;
    game->player1 = nullptr; // we can be sure that the host is the player1
    player->state = P_S_IN_MAIN_MENU;
    player->number_of_error_messages = 0;

    std::cout << "Player " << player->name << " left a lobby " << game->id << std::endl;
    send_message(fd, "LEAVE_LOBBY|OK\n");
}

void Server::list_lobbies(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);

    // Check if the number of parameters is correct
    if (!params.empty()) {
        std::cerr << "ERROR: Invalid number of parameters for LIST_LOBBIES command" << std::endl;
        send_message(fd, "LIST_LOBBIES|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player is in main menu
    if (player->state != P_S_IN_MAIN_MENU) {
        std::cerr << "ERROR: Player " << player->name << " is not in main menu" << std::endl;
        send_message(fd, "LIST_LOBBIES|ERR|You are not in main menu\n");
        player_error_message_inc(fd);
        return;
    }

    // Send the list of lobbies
    player->number_of_error_messages = 0;
    std::string message = "LIST_LOBBIES|OK";
    for (const auto& game : games) {
        if (game->state == G_S_WAITING_FOR_PLAYERS) {
            message += "|" + std::to_string(game->id);
        }
    }
    message += "\n";
    send_message(fd, message);
}

void Server::game_status(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);

    // Check if the number of parameters is correct
    if (!params.empty()) {
        std::cerr << "ERROR: Invalid number of parameters for GAME_STATUS command" << std::endl;
        send_message(fd, "GAME_STATUS|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player is in a game
    if (player->state != P_S_IN_GAME) {
        std::cerr << "ERROR: Player " << player->name << " is not in a game" << std::endl;
        send_message(fd, "GAME_STATUS|ERR|You are not in a game\n");
        player_error_message_inc(fd);
        return;
    }

    // Send the game status
    player->number_of_error_messages = 0;
    auto game = player->game;
    auto opponent = game->get_opponent(player);
    std::string message = "GAME_STATUS|OK|";
    for (auto &i : player->hand)
        message += std::to_string(i) + ",";
    message += "|";
    for (auto &i : opponent->hand)
        message += std::to_string(i) + ",";
    message += "|" + std::to_string(player->score) + "|" + std::to_string(opponent->score) + "\n";
    send_message(fd, message);
}

void Server::reroll(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);

    // Check if the number of parameters is correct
    if (params.size() != 1) {
        std::cerr << "ERROR: Invalid number of parameters for REROLL command" << std::endl;
        send_message(fd, "REROLL|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player is in a game
    if (player->state != P_S_IN_GAME) {
        std::cerr << "ERROR: Player " << player->name << " is not in a game" << std::endl;
        send_message(fd, "REROLL|ERR|You are not in a game\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player can play
    if (!player->can_play) {
        std::cerr << "ERROR: Player " << player->name << " cannot play" << std::endl;
        send_message(fd, "REROLL|ERR|You cannot play\n");
        player_error_message_inc(fd);
        return;
    }

    // Check the input
    std::vector<int> indices;
    std::stringstream ss(params[0]);
    std::string item;

    while (std::getline(ss, item, ',')) {
        try {
            int d = std::stoi(item);
            if (d < 0 || d > 1) {
                std::cerr << "ERROR: Player " << player->name << " tried to input invalid flag bit " << d << std::endl;
                send_message(fd, "REROLL|ERR|Invalid flag bit " + std::to_string(d) + "\n");
                player_error_message_inc(fd);
                return;
            }
            indices.push_back(d);
        } catch (std::invalid_argument &e) {
            std::cerr << "ERROR: Player " << player->name << " tried to input invalid flag bit " << item << std::endl;
            send_message(fd, "REROLL|ERR|Invalid flag bit " + item + "\n");
            player_error_message_inc(fd);
            return;
        }
    }

    // Check if the player has enough indices
    if (indices.size() != NUMBER_OF_DICE) {
        std::cerr << "ERROR: Player " << player->name << " tried to input more or less flag bits" << std::endl;
        send_message(fd, "REROLL|ERR|Invalid number of indices\n");
        player_error_message_inc(fd);
        return;
    }

    // Reroll the indices
    player->number_of_error_messages = 0;
    std::array<int, NUMBER_OF_DICE> arr = {
            indices[0], indices[1], indices[2], indices[3], indices[4]
    };
    player->reroll_hand(arr);

    // Send the new hand
    std::string message = "REROLL|OK|";
    for (auto &i : player->hand)
        message += std::to_string(i) + ",";
    message += "\n";
    send_message(fd, message);

    auto opponent = player->game->get_opponent(player);
    message = "REROLL_OPPONENT|OK|";
    for (auto &i : player->hand)
        message += std::to_string(i) + ",";
    message += "\n";
    send_message(opponent->socket, message);
}

void Server::run() {
    std::cout << "Server running..." << std::endl;

    // Initialize client sockets
    fd_set read_fds;
    FD_ZERO(&client_socks);
    FD_SET(server_socket, &client_socks);
    FD_SET(STDIN_FILENO, &client_socks);
    struct timeval timeout = {1, 0};

    for (;;) {
        // Copy client_socks to read_fds (select() modifies read_fds)
        read_fds = client_socks;

        // Clear char_buffer
        clear_char_buffer();

        // Wait for activity on one of the sockets
        if (select(FD_SETSIZE, &read_fds, NULL, NULL, &timeout) < 0) {
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
                        socklen_t len_addr;
                        struct sockaddr_in client_address{};
                        int client_socket = accept(server_socket, (struct sockaddr *) &client_address, &len_addr);
                        if (client_socket < 0) {
                            std::cerr << "ERR: accept" << std::endl;
                            continue;
                        }
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

                        // Client disconnected (not on purpose)
                        } else {
                            auto player = get_player_by_fd(fd);
                            std::cout << "Client " <<fd << " (" << player->name << ") suddenly disconnected" << std::endl;
                            player->logged_in = false;
                            player->state = P_S_DISCONNECTED;
                            player->disconnect_time = std::chrono::high_resolution_clock::now();
                            close(fd);
                            FD_CLR(fd, &client_socks);
                            disconnected_players.push_back(player);
                            players.erase(std::remove_if(players.begin(), players.end(),
                                                         [fd](const std::shared_ptr<Player>& player) {
                                                             return player->socket == fd;
                                                         }), players.end());
                        }
                    }
                }
            }
        }

        // Check if there are any new rounds in the games
        for (auto &i : games) {
            if (i->state == G_S_PLAYING && i->is_new_round) {
                i->is_new_round = false;
                game_status(i->player1->socket, {});
                game_status(i->player2->socket, {});
            }
        }

        // Check if there are any players to be deleted
        for (auto &i : disconnected_players) {
            auto now = std::chrono::high_resolution_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - i->disconnect_time).count();
            if (diff > DC_TIMEOUT_SEC) {
                std::cout << "Player " << i->name << " timed out" << std::endl;
                disconnected_players.erase(std::remove_if(disconnected_players.begin(), disconnected_players.end(),
                                                          [i](const std::shared_ptr<Player>& player) {
                                                              return player == i;
                                                          }), disconnected_players.end());
            }
        }

        // Check if there are any games to be deleted
        for (auto &i : games) {
            bool is_to_be_deleted = true;
            auto player1 = i->player1;
            auto player2 = i->player2;
            // Check if there is at least one player in the game
            for (auto &j : players) {
                if (player1 == j || player2 == j) {
                    is_to_be_deleted = false;
                    break;
                }
            }
            for (auto &j : disconnected_players) {
                if (player1 == j || player2 == j) {
                    is_to_be_deleted = false;
                    break;
                }
            }
            // No player found, game is empty
            if (is_to_be_deleted) {
                std::cout << "Game " << i->id << " deleted, no players are connected" << std::endl;
                games.erase(std::remove_if(games.begin(), games.end(),
                                           [i](const std::shared_ptr<Game>& game) {
                                               return game->id == i->id;
                                           }), games.end());
            }
        }

    }

    std::cout << "Server shutting down..." << std::endl;
}
