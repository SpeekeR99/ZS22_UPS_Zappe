#include "Server.h"

Server::Server(const std::string &ip, int port) : game_id(0), random{} {
    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Set socket options
    int param = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char *) &param, sizeof(int)) == -1) {
        log("ERROR: setsockopt");
        exit(EXIT_FAILURE);
    }
    else
        log("OK: setsockopt");

    // Set server address
    struct sockaddr_in server_address{};
    memset(&server_address, 0, sizeof(struct sockaddr_in));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (ip.empty())
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        server_address.sin_addr.s_addr = inet_addr(ip.c_str());

    // Bind socket to address
    if (!bind(server_socket, (struct sockaddr *) &server_address, sizeof(struct sockaddr_in)))
        log("OK: bind");
    else {
        log("ERROR: bind");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (!listen(server_socket, 5))
        log("OK: listen");
    else {
        log("ERROR: listen");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    getsockname(server_socket, (struct sockaddr *)&sin, &len);

    log("Server started on port " + std::to_string(ntohs(sin.sin_port)));

    init_commands_map();
    random = std::make_unique<MyRandom>();
}

Server::~Server() {
    close(server_socket);
}

void Server::log(const std::string &message) {
    auto time = std::time(nullptr);
    auto time_string = std::ctime(&time);
    time_string[std::string((time_string)).size() - 1] = '\0';

    auto log = std::ofstream(LOG_FILE, std::ios::app);
    log << "[" << time_string << "]: " << message << std::endl;
    log.close();

    if (message.find("ERR") != std::string::npos)
        std::cerr << "[" << time_string << "]: " << message << std::endl;
    else
        std::cout << "[" << time_string << "]: " << message << std::endl;
}

void Server::init_commands_map() {
    commands["OK"] = &Server::ok_response;
    commands["ALIVE"] = &Server::alive;
    commands["HELLO"] = &Server::handshake;
    commands["LOGIN"] = &Server::login;
    commands["LOGOUT"] = &Server::logout;
    commands["RECONNECT"] = &Server::reconnect;
    commands["CREATE_GAME"] = &Server::create_game;
    commands["JOIN_GAME"] = &Server::join_game;
    commands["LEAVE_GAME"] = &Server::leave_game;
    commands["LIST_GAMES"] = &Server::list_games;
    commands["GAME_STATUS"] = &Server::game_status;
    commands["REROLL"] = &Server::reroll;
    commands["ACCEPT_END_OF_ROUND"] = &Server::accept_end_of_round;
}

void Server::clear_char_buffer() {
    for (char &i: char_buffer)
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

bool Server::recv_message(int fd, int a2read) {
    if (a2read >= BUFFER_LEN) {
        log("ERROR: Client " + std::to_string(fd) + " has been disconnected for sending too long message");
        send_message(fd, "ERR: Too long message. Disconnecting...\n");
        disconnect_player(fd);
        return false;
    }
    recv(fd, char_buffer, a2read, 0);
    buffer = std::string(char_buffer);
    clear_char_buffer();
    return true;
}

std::vector<std::string> Server::tokenize_buffer(char delim) const {
    std::stringstream ss(buffer);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, delim))
        tokens.push_back(token);

    return tokens;
}

std::shared_ptr<Player> Server::get_player_by_fd(int fd) {
    std::shared_ptr<Player> player;
    for (auto &i: players)
        if (i->socket == fd) player = i;
    return player;
}

std::shared_ptr<Game> Server::get_game_by_id(uint32_t id) {
    std::shared_ptr<Game> game;
    for (auto &i: games)
        if (i->id == id) game = i;
    return game;
}

bool Server::is_name_taken(const std::string &name) {
    return std::any_of(players.begin(), players.end(), [&name](const std::shared_ptr<Player> &player) {
        return player->name == name;
    }) || std::any_of(disconnected_players.begin(), disconnected_players.end(),
                      [&name](const std::shared_ptr<Player> &player) {
                          return player->name == name;
                      });
}

bool Server::does_game_exist(uint32_t id) {
    return std::any_of(games.begin(), games.end(), [id](const std::shared_ptr<Game> &game) {
        return game->id == id;
    });
}

void Server::disconnect_player(int fd) {
    auto player = get_player_by_fd(fd);
    auto game = player->game;

    if (game) {
        if (game->player1 == player) game->player1 = nullptr;
        else if (game->player2 == player) game->player2 = nullptr;

        player->game = nullptr;

        auto opponent = game->get_opponent(player);
        if (opponent && opponent->state != P_S_DISCONNECTED) {
            game_status(opponent->socket, {});
            nanosleep((const struct timespec[]){{0, NANOSEC_IN_SEC / 4}}, nullptr);
            game_over(opponent->socket);
        }
    }

    log("Client " + std::to_string(fd) + " disconnected");
    close(fd);
    FD_CLR(fd, &client_socks);
    players.erase(std::remove_if(players.begin(), players.end(),
                                 [fd](const std::shared_ptr<Player> &player) {
                                     return player->socket == fd;
                                 }), players.end());
}

void Server::player_error_message_inc(int fd) {
    auto player = get_player_by_fd(fd);
    player->number_of_error_messages++;

    // Disconnect player if he sent too many error messages
    if (player->number_of_error_messages >= 3) {
        log("Player " + player->name + " has been disconnected for too many error messages");
        send_message(fd, "Too many error messages. Disconnecting...\n");
        disconnect_player(fd);
    }
}

void Server::handle_incoming_message(int fd) {
    auto player = get_player_by_fd(fd);

    log("Received message from client " + std::to_string(fd) + " (" + player->name + "): " + buffer);

    auto tokens = tokenize_buffer(COMMAND_DELIMITER);
    std::string cmd = tokens[0];
    tokens.erase(tokens.begin());
    cmd.erase(std::remove_if(cmd.begin(), cmd.end(), ::isspace), cmd.end());

    // Check if command exists
    if (commands.count(cmd)) {
        // Check if player has sent HELLO message and is logged in (or is trying to send HELLO message / login / reconnect)
        if ((player->handshake && player->logged_in) || cmd == "HELLO" || cmd == "LOGIN" || cmd == "RECONNECT" ||
            cmd == "ALIVE")
            (this->*commands[cmd])(fd, tokens);
        else {
            log("ERROR: Client " + std::to_string(fd) + " (" + player->name +
                ") tried to send message before handshake or login");
            send_message(fd, "CMD|ERR|You must handshake and login first\n");
            player_error_message_inc(fd);
        }
    } else {
        log("ERROR: Client " + std::to_string(fd) + " (" + player->name + ") sent unknown command");
        send_message(fd, "CMD|ERR|Unknown command|" + cmd + "\n");
        player_error_message_inc(fd);
    }
}

void Server::ok_response(int fd, const std::vector<std::string> &args) {
    // Do nothing about OK response
}

void Server::alive(int fd, const std::vector<std::string> &params) {
    // Check if the number of parameters is correct
    if (!params.empty()) {
        log("ERROR: Invalid number of parameters for ALIVE command");
        send_message(fd, "ALIVE|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    get_player_by_fd(fd)->number_of_error_messages = 0;
    log("Sending ALIVE response to client " + std::to_string(fd));
    send_message(fd, "ALIVE\n");
}

void Server::handshake(int fd, const std::vector<std::string> &params) {
    // Check if the number of parameters is correct
    if (!params.empty()) {
        log("ERROR: Invalid number of parameters for HELLO command");
        send_message(fd, "HELLO|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    get_player_by_fd(fd)->number_of_error_messages = 0;
    log("Sending HELLO response to client " + std::to_string(fd));
    send_message(fd, "HELLO\n");
    get_player_by_fd(fd)->handshake = true;
}

void Server::login(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);
    auto name = params[0];
    name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());

    // Check if the number of parameters is correct
    if (params.size() != 1) {
        log("ERROR: Invalid number of parameters for LOGIN command");
        send_message(fd, "LOGIN|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player is not already logged in
    if (player->logged_in) {
        log("ERROR: Player " + player->name + " is already logged in");
        send_message(fd, "LOGIN|ERR|You are already logged in\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the name is valid (not too long)
    if (name.size() > MAX_NAME_LEN) {
        log("ERROR: Nickname cannot be longer than " + std::to_string(MAX_NAME_LEN) + " characters");
        send_message(fd, "LOGIN|ERR|Nickname cannot be longer than " + std::to_string(MAX_NAME_LEN) + " characters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the name is not taken
    if (is_name_taken(name)) {
        log("ERROR: Nickname " + name + " is already taken");
        send_message(fd, "LOGIN|ERR|Nickname is already taken\n");
        return;
    }

    player->name = name;
    player->logged_in = true;
    player->number_of_error_messages = 0;

    log("Player " + player->name + " logged in");
    send_message(fd, "LOGIN|OK\n");
}

void Server::logout(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);

    // Check if the number of parameters is correct
    if (!params.empty()) {
        log("ERROR: Invalid number of parameters for GOODBYE command");
        send_message(fd, "GOODBYE|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    if (player->game) {
        leave_game(fd, {});
        nanosleep((const struct timespec[]){{0, NANOSEC_IN_SEC / 4}}, nullptr);
    }

    log("Player " + player->name + " logged out");
    send_message(fd, "GOODBYE\n");

    close(fd);
    FD_CLR(fd, &client_socks);
    players.erase(std::remove_if(players.begin(), players.end(),
                                 [fd](const std::shared_ptr<Player> &player) {
                                     return player->socket == fd;
                                 }), players.end());
}

void Server::reconnect(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);
    auto name = params[0];
    name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());

    // Check if the number of parameters is correct
    if (params.size() != 1) {
        log("ERROR: Invalid number of parameters for RECONNECT command");
        send_message(fd, "RECONNECT|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the name exists
    if (!is_name_taken(name)) {
        log("ERROR: Player " + name + " does not exist");
        send_message(fd, "RECONNECT|ERR|Name does not exist\n");
        player_error_message_inc(fd);
        return;
    }

    std::shared_ptr<Player> player_reconnecting_to;
    for (auto &i: players) {
        if (i->name == name) {
            player_reconnecting_to = i;
            break;
        }
    }
    for (auto &i: disconnected_players) {
        if (i->name == name) {
            player_reconnecting_to = i;
            break;
        }
    }

    // Check if the destination player is really disconnected
    if (player_reconnecting_to->logged_in || player_reconnecting_to->state != P_S_DISCONNECTED) {
        log("ERROR: Player " + name + " is not disconnected");
        send_message(fd, "RECONNECT|ERR|Player is not disconnected\n");
        player_error_message_inc(fd);
        return;
    }

    // Restore the player
    player->name = name;
    player->game = player_reconnecting_to->game;
    player->hand = player_reconnecting_to->hand;
    player->score = player_reconnecting_to->score;
    player->random = random;
    player->can_play = player_reconnecting_to->can_play;
    player->logged_in = true;
    player->accepted_end_of_round = player_reconnecting_to->accepted_end_of_round;
    player->number_of_error_messages = 0;
    if (player->game) {
        player->game->paused = false;
        if (player->game->state == G_S_WAITING_FOR_PLAYERS)
            player->state = P_S_IN_LOBBY;
        else
            player->state = P_S_IN_GAME;

        if (player->game->player1 == player_reconnecting_to)
            player->game->player1 = player;
        else
            player->game->player2 = player;

        if (player->game->get_opponent(player)) {
            log("Player " + player->name + " reconnected to game " + std::to_string(player->game->id));
            send_message(player->game->get_opponent(player)->socket, "OPPONENT_RECONNECTED\n");
            nanosleep((const struct timespec[]){{0, NANOSEC_IN_SEC / 4}}, nullptr);
        }
    } else
        player->state = P_S_IN_MAIN_MENU;

    // Remove the player from the disconnected players list
    disconnected_players.erase(std::remove_if(disconnected_players.begin(), disconnected_players.end(),
                                              [player_reconnecting_to](const std::shared_ptr<Player> &player) {
                                                  return player == player_reconnecting_to;
                                              }), disconnected_players.end());

    log("Player " + player->name + " reconnected");
    send_message(fd, "RECONNECT|OK\n");

    // Send the game state if the player is in a game and the opponent is still connected too
    if (player->game && player->game->get_opponent(player)) {
        nanosleep((const struct timespec[]){{0, NANOSEC_IN_SEC / 4}}, nullptr);
        game_status(fd, {});
    }
        // Game exists, but opponent has already left, so it's pointless to connect back to the game
    else if (player->game && !player->game->get_opponent(player))
        leave_game(fd, {});
}

void Server::create_game(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);

    // Check if the number of parameters is correct
    if (!params.empty()) {
        log("ERROR: Invalid number of parameters for CREATE_GAME command");
        send_message(fd, "CREATE_GAME|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player is in main menu
    if (player->state != P_S_IN_MAIN_MENU) {
        log("ERROR: Player " + player->name + " is not in main menu");
        send_message(fd, "CREATE_GAME|ERR|You are not in main menu\n");
        player_error_message_inc(fd);
        return;
    }

    // Create a new lobby
    player->number_of_error_messages = 0;
    auto game = std::make_shared<Game>(game_id++, player);
    player->game = game;
    games.push_back(game);

    log("Player " + player->name + " created a game " + std::to_string(game->id));
    send_message(fd, "CREATE_GAME|OK|" + std::to_string(game->id) + "\n");
}

void Server::join_game(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);

    // Check if the number of parameters is correct
    if (params.size() != 1) {
        log("ERROR: Invalid number of parameters for JOIN_GAME command");
        send_message(fd, "JOIN_GAME|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player is in main menu
    if (player->state != P_S_IN_MAIN_MENU) {
        log("ERROR: Player " + player->name + " is not in main menu");
        send_message(fd, "JOIN_GAME|ERR|You are not in main menu\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the lobby exists
    auto id = std::stoi(params[0]);
    if (!does_game_exist(id)) {
        log("ERROR: Game " + std::to_string(id) + " does not exist");
        send_message(fd, "JOIN_GAME|ERR|Game does not exist\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the lobby is full
    auto game = get_game_by_id(id);
    if (game->state != G_S_WAITING_FOR_PLAYERS) {
        log("ERROR: Game " + std::to_string(id) + " is full");
        send_message(fd, "JOIN_GAME|ERR|Game is full\n");
        player_error_message_inc(fd);
        return;
    }

    // Join the lobby
    player->number_of_error_messages = 0;
    game->join_game(player);
    log("Player " + player->name + " joined game " + std::to_string(game->id));

    // Notify both players about their opponent
    auto opponent = game->get_opponent(player);
    send_message(fd, "JOIN_GAME|OK|" + opponent->name + "\n");
    send_message(opponent->socket, "JOIN_GAME|OK|" + player->name + "\n");

    log("Game " + std::to_string(game->id) + " started");
}

void Server::leave_game(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);

    // Check if the number of parameters is correct
    if (!params.empty()) {
        log("ERROR: Invalid number of parameters for LEAVE_GAME command");
        send_message(fd, "LEAVE_GAME|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player is in a game
    if (player->state != P_S_IN_LOBBY && player->state != P_S_IN_GAME) {
        log("ERROR: Player " + player->name + " is not in a game");
        send_message(fd, "LEAVE_GAME|ERR|You are not in a game\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the game is waiting for players
    player->number_of_error_messages = 0;
    auto game = player->game;
    if (game->state == G_S_WAITING_FOR_PLAYERS) {
        game->player1 = nullptr; // we can be sure that the host is the player1
        player->state = P_S_IN_MAIN_MENU;

        log("Player " + player->name + " left a game " + std::to_string(game->id));
        send_message(fd, "LEAVE_GAME|OK\n");
    }
        // Game is in the middle of the playing or is paused
    else {
        auto opponent = game->get_opponent(player);
        if (opponent && opponent->state != P_S_DISCONNECTED)
            game_status(opponent->socket, {});

        if (game->player1 == player) {
            game->player1 = nullptr;
        } else {
            game->player2 = nullptr;
        }

        player->game = nullptr;
        player->state = P_S_IN_MAIN_MENU;
        player->can_play = false;

        log("Player " + player->name + " left a game " + std::to_string(game->id));
        send_message(fd, "LEAVE_GAME|OK\n");
        if (opponent && opponent->state != P_S_DISCONNECTED) {
            send_message(opponent->socket, "LEAVE_GAME_OPPONENT|OK\n");
            if (game->state != G_S_FINISHED) {
                game->state = G_S_FINISHED;
                game->game_over = false;
                nanosleep((const struct timespec[]){{0, NANOSEC_IN_SEC / 4}}, nullptr);
                game_over(opponent->socket);
            }
        }
    }
}

void Server::list_games(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);

    // Check if the number of parameters is correct
    if (!params.empty()) {
        log("ERROR: Invalid number of parameters for LIST_GAMES command");
        send_message(fd, "LIST_GAMES|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player is in main menu
    if (player->state != P_S_IN_MAIN_MENU) {
        log("ERROR: Player " + player->name + " is not in main menu");
        send_message(fd, "LIST_GAMES|ERR|You are not in main menu\n");
        player_error_message_inc(fd);
        return;
    }

    // Send the list of games
    player->number_of_error_messages = 0;
    std::string message = "LIST_GAMES|OK";
    for (const auto &game: games) {
        if (game->state == G_S_WAITING_FOR_PLAYERS)
            message += "|" + std::to_string(game->id) + "," + game->player1->name;
    }
    message += "\n";
    log("Player " + player->name + " requested a list of games");
    send_message(fd, message);
}

void Server::game_status(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);

    // Check if the number of parameters is correct
    if (!params.empty()) {
        log("ERROR: Invalid number of parameters for GAME_STATUS command");
        send_message(fd, "GAME_STATUS|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player is in a game
    if (player->state != P_S_IN_GAME) {
        log("ERROR: Player " + player->name + " is not in a game");
        send_message(fd, "GAME_STATUS|ERR|You are not in a game\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the opponent exists
    if (!player->game->get_opponent(player)) {
        log("ERROR: Opponent of " + player->name + " is no longer in the game");
        send_message(fd, "GAME_STATUS|ERR|Opponent already left, cannot access information about him\n");
        return;
    }

    // Send the game status
    player->number_of_error_messages = 0;
    auto game = player->game;
    auto opponent = game->get_opponent(player);
    std::string message = "GAME_STATUS|OK|";
    for (auto &i: player->hand)
        message += std::to_string(i) + ",";
    message = message.substr(0, message.size() - 1);
    message += "|";
    for (auto &i: opponent->hand)
        message += std::to_string(i) + ",";
    message = message.substr(0, message.size() - 1);
    message += "|" + std::to_string(player->score) + "|" + std::to_string(opponent->score) + "|" +
               std::to_string(player->can_play) + "|" + std::to_string(opponent->can_play) + "|" + opponent->name +
               "\n";
    log("Player " + player->name + " requested a game status");
    send_message(fd, message);
}

void Server::reroll(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);

    // Check if the number of parameters is correct
    if (params.size() != 1) {
        log("ERROR: Invalid number of parameters for REROLL command");
        send_message(fd, "REROLL|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player is in a game
    if (player->state != P_S_IN_GAME) {
        log("ERROR: Player " + player->name + " is not in a game");
        send_message(fd, "REROLL|ERR|You are not in a game\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player can play
    if (!player->can_play) {
        log("ERROR: Player " + player->name + " cannot play");
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
                log("ERROR: Player " + player->name + " tried to input invalid flag bit " + std::to_string(d));
                send_message(fd, "REROLL|ERR|Invalid flag bit " + std::to_string(d) + "\n");
                player_error_message_inc(fd);
                return;
            }
            indices.push_back(d);
        } catch (std::invalid_argument &e) {
            log("ERROR: Player " + player->name + " tried to input invalid flag bit " + item);
            send_message(fd, "REROLL|ERR|Invalid flag bit " + item + "\n");
            player_error_message_inc(fd);
            return;
        }
    }

    // Check if the player input the right amount of flags
    if (indices.size() != NUMBER_OF_DICE) {
        log("ERROR: Player " + player->name + " tried to input more or less flag bits");
        send_message(fd, "REROLL|ERR|Invalid number of flag bits\n");
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
    for (auto &i: player->hand)
        message += std::to_string(i) + ",";
    message = message.substr(0, message.size() - 1);
    message += "\n";
    log("Player " + player->name + " rerolled the dice");
    send_message(fd, message);

    if (player->game->paused)
        return;

    auto opponent = player->game->get_opponent(player);
    message = "REROLL_OPPONENT|OK|";
    for (auto &i: player->hand)
        message += std::to_string(i) + ",";
    message = message.substr(0, message.size() - 1);
    message += "\n";
    send_message(opponent->socket, message);
}

void Server::accept_end_of_round(int fd, const std::vector<std::string> &params) {
    auto player = get_player_by_fd(fd);

    // Check if the number of parameters is correct
    if (!params.empty()) {
        log("ERROR: Invalid number of parameters for ACCEPT_END_OF_ROUND command");
        send_message(fd, "ACCEPT_END_OF_ROUND|ERR|Invalid number of parameters\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the player is in a game
    if (player->state != P_S_IN_GAME) {
        log("ERROR: Player " + player->name + " is not in a game");
        send_message(fd, "ACCEPT_END_OF_ROUND|ERR|You are not in a game\n");
        player_error_message_inc(fd);
        return;
    }

    // Check if the game is at the end of round
    auto game = player->game;
    if (!game->is_end_round) {
        log("ERROR: Game " + std::to_string(game->id) + " is not at the end of round");
        send_message(fd, "ACCEPT_END_OF_ROUND|ERR|Game is not at the end of round\n");
        player_error_message_inc(fd);
        return;
    }

    //Check if the player did not already accept the end of round
    if (player->accepted_end_of_round) {
        log("ERROR: Player " + player->name + " already accepted the end of round");
        send_message(fd, "ACCEPT_END_OF_ROUND|ERR|You already accepted the end of round\n");
        player_error_message_inc(fd);
        return;
    }

    // Accept the end of round
    player->number_of_error_messages = 0;
    player->accepted_end_of_round = true;
    log("Player " + player->name + " accepted the end of round");
    send_message(fd, "ACCEPT_END_OF_ROUND|OK\n");

    auto opponent = game->get_opponent(player);
    if (opponent && opponent->state != P_S_DISCONNECTED && opponent->accepted_end_of_round) {
        game->is_end_round = false;
        game->start_round();
    }
}

void Server::game_over(int fd) {
    auto player = get_player_by_fd(fd);

    auto game = player->game;
    auto opponent = game->get_opponent(player);

    // Check if opponent exists
    if (opponent && opponent->state != P_S_DISCONNECTED) {
        if (player->score > opponent->score)
            send_message(fd, "GAME_OVER|OK|WIN\n");
        else if (player->score < opponent->score)
            send_message(fd, "GAME_OVER|OK|LOSS\n");
        else
            send_message(fd, "GAME_OVER|OK|DRAW\n");
    }
        // Player is the winner, because opponent doesn't exist
    else
        send_message(fd, "GAME_OVER|OK|WIN\n");

    log("Game " + std::to_string(game->id) + " is over");
}

void Server::run() {
    log("Server running...");

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
            log("ERROR: select");
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
                        struct sockaddr_in client_address{};
                        socklen_t len_addr = sizeof((struct sockaddr *) &client_address);
                        int client_socket = accept(server_socket, (struct sockaddr *) &client_address, &len_addr);
                        if (client_socket < 0) {
                            log("ERROR: accept");
                            continue;
                        }
                        FD_SET(client_socket, &client_socks);
                        auto player = std::make_shared<Player>(client_socket);
                        player->random = this->random;
                        players.push_back(player);
                        log("New client connected: " + std::to_string(client_socket));

                        // It is a message from a client
                    } else {
                        int a2read;
                        ioctl(fd, FIONREAD, &a2read);

                        // Client sent some bytes
                        if (a2read > 0) {
                            if (recv_message(fd, a2read))
                                handle_incoming_message(fd);

                            // Client disconnected (not on purpose)
                        } else {
                            auto player = get_player_by_fd(fd);
                            log("Client " + std::to_string(fd) + " (" + player->name + ") suddenly disconnected");
                            player->logged_in = false;
                            player->state = P_S_DISCONNECTED;
                            auto game = player->game;
                            if (game) {
                                game->paused = true;
                                auto opponent = game->get_opponent(player);
                                if (opponent && opponent->state != P_S_DISCONNECTED)
                                    send_message(opponent->socket, "OPPONENT_DISCONNECTED\n");
                            }
                            player->disconnect_time = std::chrono::high_resolution_clock::now();
                            close(fd);
                            FD_CLR(fd, &client_socks);
                            disconnected_players.push_back(player);
                            players.erase(std::remove_if(players.begin(), players.end(),
                                                         [fd](const std::shared_ptr<Player> &player) {
                                                             return player->socket == fd;
                                                         }), players.end());
                        }
                    }
                }
            }
        }

        // Check if there are any new rounds in the games
        for (auto &i: games) {
            if (i->state == G_S_PLAYING && i->is_new_round) {
                i->is_new_round = false;
                auto player1 = i->player1;
                auto player2 = i->player2;
                if (player1 && player1->state != P_S_DISCONNECTED)
                    game_status(player1->socket, {});
                if (player2 && player2->state != P_S_DISCONNECTED)
                    game_status(player2->socket, {});
            }
        }

        // Check if there are any end of rounds in the games
        for (auto &i: games) {
            if (i->state == G_S_PLAYING && i->send_end_of_round) {
                i->send_end_of_round = false;
                auto player1 = i->player1;
                auto player2 = i->player2;
                if (player1 && player1->state != P_S_DISCONNECTED)
                    game_status(player1->socket, {});
                if (player2 && player2->state != P_S_DISCONNECTED)
                    game_status(player2->socket, {});
            }
        }

        // Check if there are any finished games
        for (auto &i: games) {
            if (i->state == G_S_FINISHED && i->game_over) {
                i->game_over = false;
                auto player1 = i->player1;
                auto player2 = i->player2;
                if (player1 && player1->state != P_S_DISCONNECTED)
                    game_status(player1->socket, {});
                if (player2 && player2->state != P_S_DISCONNECTED)
                    game_status(player2->socket, {});
                nanosleep((const struct timespec[]){{0, NANOSEC_IN_SEC / 4}}, nullptr);
                if (player1 && player1->state != P_S_DISCONNECTED)
                    game_over(player1->socket);
                if (player2 && player2->state != P_S_DISCONNECTED)
                    game_over(player2->socket);
            }
        }

        // Check if there are any players to be deleted
        for (auto it = disconnected_players.begin(); it != disconnected_players.end();) {
            bool removed = false;
            auto now = std::chrono::high_resolution_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - (*it)->disconnect_time).count();
            if (diff > DC_TIMEOUT_SEC) {
                log("Player " + (*it)->name + " timed out");
                auto game = (*it)->game;
                if (game) {
                    game->paused = false;
                    game->game_over = false;
                    game->state = G_S_FINISHED;

                    auto opponent = game->get_opponent((*it));
                    if (opponent && opponent->state != P_S_DISCONNECTED) {
                        game_status(opponent->socket, {});
                        nanosleep((const struct timespec[]){{0, NANOSEC_IN_SEC / 4}}, nullptr);
                    }

                    if ((*it) == game->player1)
                        game->player1 = nullptr;
                    else
                        game->player2 = nullptr;

                    if (opponent && opponent->state != P_S_DISCONNECTED)
                        game_over(opponent->socket);
                }
                it = disconnected_players.erase(it);
                removed = true;
            }
            if (!removed)
                ++it;
        }

        // Check if there are any games to be deleted
        for (auto it = games.begin(); it != games.end();) {
            bool removed = false;
            bool is_to_be_deleted = true;
            auto player1 = (*it)->player1;
            auto player2 = (*it)->player2;
            // Check if there is at least one player in the game
            for (auto &j: players) {
                if (player1 == j || player2 == j) {
                    is_to_be_deleted = false;
                    break;
                }
            }
            for (auto &j: disconnected_players) {
                if (player1 == j || player2 == j) {
                    is_to_be_deleted = false;
                    break;
                }
            }

            // No player found, game is empty
            if (is_to_be_deleted) {
                log("Game " + std::to_string((*it)->id) + " deleted, no players are connected");
                it = games.erase(it);
                removed = true;
            }

            if (!removed)
                ++it;
        }

    }

    log("Server shutting down...");
}
