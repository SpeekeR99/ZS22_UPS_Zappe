#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <fstream>
#include <ctime>
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

/** Size of the buffer used for communication between server and client */
constexpr size_t BUFFER_LEN = 1024;
/** Maximum size of the player name */
constexpr size_t MAX_NAME_LEN = 20;
/** Separator of the commands and parameters */
constexpr char COMMAND_DELIMITER = '|';
/** Maximum time in seconds to wait for a client to reconnect */
constexpr uint32_t DC_TIMEOUT_SEC = 30;
/** Filepath to the log file */
constexpr const char *LOG_FILE = "server.log";

/**
 * Class that represents the server
 * It handles the communication with the clients and the game logic
 */
class Server {
private:
    /** Typedef of function pointer to a function that handles a command */
    typedef void (Server::*command)(int, const std::vector<std::string> &);
    /** Typedef of map that maps a string command to a function pointer */
    typedef std::map<std::string, command> command_map;

    /** Server socket */
    int server_socket;
    /** Set of client sockets */
    fd_set client_socks{};
    /** Buffer for communication */
    char char_buffer[BUFFER_LEN]{};
    /** Buffer for communication (string version for easier usage) */
    std::string buffer;
    /** Random number generator */
    std::shared_ptr<MyRandom> random;
    /** Vector of players that are currently connected */
    std::vector<std::shared_ptr<Player>> players;
    /** Vector of players that are currently disconnected */
    std::vector<std::shared_ptr<Player>> disconnected_players;
    /** Unique game id */
    uint32_t game_id;
    /** Vector of games that currently exist */
    std::vector<std::shared_ptr<Game>> games;
    /** Map of commands to their corresponding function pointer */
    command_map commands;

    /**
     * Function that handles logging to the stdout (or stderr) and the log file
     * @param message Message to log
     */
    void log(const std::string &message);

    /**
     * Function that initializes the command map
     * Binds a string command to a function pointer
     * Fills the commands command_map member attribute
     */
    void init_commands_map();

    /**
     * Function that clears the buffer
     * Sets all characters to '\0'
     */
    void clear_char_buffer();

    /**
     * Function that reads a message from a file descriptor (used for stdin)
     * Message is stored in the buffer member attribute
     * @param fd File descriptor of the socket
     */
    void read_message(int fd);

    /**
     * Function that sends a given message to a given socket
     * @param fd File descriptor of the socket
     * @param message Message to be sent
     */
    void send_message(int fd, const std::string &message);

    /**
     * Function that recieves a message from a given socket
     * Message is stored in the buffer member attribute
     * @param fd File descriptor of the socket
     * @param a2read Number of bytes to read
     * @return True if the message was read successfully, false otherwise
     */
    bool recv_message(int fd, int a2read);

    /**
     * Function that tokenizes the buffer member attribute by a given delimiter (separator)
     * @param delim Delimeter (separator) to tokenize by (default is COMMAND_DELIMITER)
     * @return Vector of tokens (strings)
     */
    std::vector<std::string> tokenize_buffer(char delim) const;

    /**
     * Function that finds an existing player by a given file descriptor
     * @param fd File descriptor of the socket
     * @return Shared pointer to the player
     */
    std::shared_ptr<Player> get_player_by_fd(int fd);

    /**
     * Function that finds an existing game by a given game id
     * @param id Game id (unique)
     * @return Shared pointer to the game
     */
    std::shared_ptr<Game> get_game_by_id(uint32_t id);

    /**
     * Function that checks if a given nickname is already taken
     * @param name Nickname to check
     * @return True if nickname is taken, false otherwise
     */
    bool is_name_taken(const std::string &name);

    /**
     * Function that checks if a given game (given by id) exists
     * @param id Game id (unique)
     * @return True if game exists, false otherwise
     */
    bool does_game_exist(uint32_t id);

    /**
     * Function that disconnects a given player (given by file descriptor)
     * @param fd File descriptor of the socket
     */
    void disconnect_player(int fd);

    /**
     * Function that handles incrementing of the error counter of a given player (given by file descriptor)
     * @param fd File descriptor of the socket
     */
    void player_error_message_inc(int fd);

    /**
     * Function that handles all incoming messages from the clients
     * If the message is a known command, it calls the corresponding function (thanks to the command map)
     * @param fd File descriptor of the socket
     */
    void handle_incoming_message(int fd);

    /**
     * Function that just receives the OK message from the client
     * Does nothing at all, because OK response is not guaranteed because of NetCat player :)
     * @param fd File descriptor of the socket
     * @param params Params of the command (not used)
     */
    void ok_response(int fd, const std::vector<std::string> &params);

    /**
     * Function that handles the "ALIVE" command
     * C -> S : ALIVE
     * S -> C : ALIVE
     * @param fd File descriptor of the socket
     * @param params Params of the command
     */
    void alive(int fd, const std::vector<std::string> &params);

    /**
     * Function that handles the "HELLO" command
     * C -> S : HELLO
     * S -> C : HELLO
     * @param fd File descriptor of the socket
     * @param params Params of the command
     */
    void handshake(int fd, const std::vector<std::string> &params);

    /**
     * Function that handles the "LOGIN" command
     * C -> S : LOGIN|<nickname>
     * S -> C : LOGIN|OK
     * S -> C : LOGIN|ERR|<error message>
     * @param fd File descriptor of the socket
     * @param params Params of the command
     *               params[0] is the nickname
     */
    void login(int fd, const std::vector<std::string> &params);

    /**
     * Function that handles the "LOGOUT" command
     * C -> S : LOGOUT
     * S -> C : GOODBYE
     * @param fd File descriptor of the socket
     * @param params Params of the command
     */
    void logout(int fd, const std::vector<std::string> &params);

    /**
     * Function that handles the "RECONNECT" command
     * C -> S : RECONNECT|<nickname>
     * S -> C : RECONNECT|OK
     * S -> C : RECONNECT|ERR|<error message>
     * @param fd File descriptor of the socket
     * @param params Params of the command
     *               params[0] is the nickname
     */
    void reconnect(int fd, const std::vector<std::string> &params);

    /**
     * Function that handles the "CREATE_GAME" command
     * C -> S : CREATE_GAME
     * S -> C : CREATE_GAME|OK|<game id>
     * @param fd File descriptor of the socket
     * @param params Params of the command
     */
    void create_game(int fd, const std::vector<std::string> &params);

    /**
     * Function that handles the "JOIN_GAME" command
     * C -> S : JOIN_GAME|<game id>
     * S -> C : JOIN_GAME|OK|<opponent nickname>
     * S -> C : JOIN_GAME|ERR|<error message>
     * @param fd File descriptor of the socket
     * @param params Params of the command
     *               params[0] is the game id
     */
    void join_game(int fd, const std::vector<std::string> &params);

    /**
     * Function that handles the "LEAVE_GAME" command
     * C -> S : LEAVE_GAME
     * S -> C : LEAVE_GAME|OK
     * S -> C : LEAVE_GAME|ERR|<error message>
     * @param fd File descriptor of the socket
     * @param params Params of the command
     */
    void leave_game(int fd, const std::vector<std::string> &params);

    /**
     * Function that handles the "LIST_GAMES" command
     * C -> S : LIST_GAMES
     * S -> C : LIST_GAMES|OK|<game id 1>,<game id 2>,...
     * @param fd File descriptor of the socket
     * @param params Params of the command
     */
    void list_games(int fd, const std::vector<std::string> &params);

    /**
     * Function that handles the "GAME_STATUS" command
     * C -> S : GAME_STATUS
     * S -> C : GAME_STATUS|OK|<player hand>|<opponent hand>|<player points>|<opponent points>|<player can play>|<opponent can play>|<oppoent nickname>
     * S -> C : GAME_STATUS|ERR|<error message>
     * @param fd
     * @param params
     */
    void game_status(int fd, const std::vector<std::string> &params);

    /**
     * Function that handles the "REROLL" command
     * C -> S : REROLL|<flag of die 1>,<flag of die 2>,<flag of die 3>,<flag of die 4>,<flag of die 5>
     * S -> C : REROLL|OK|<new hand>
     * S -> C : REROLL|ERR|<error message>
     * @param fd File descriptor of the socket
     * @param params Params of the command
     *        params[0] are the bit flags of the dice to reroll
     */
    void reroll(int fd, const std::vector<std::string> &params);

    /**
     * Function that handles the "ACCEPT_END_OF_ROUND" command
     * C -> S : ACCEPT_END_OF_ROUND
     * S -> C : ACCEPT_END_OF_ROUND|OK
     * S -> C : ACCEPT_END_OF_ROUND|ERR|<error message>
     * @param fd File descriptor of the socket
     * @param params Params of the command
     */
    void accept_end_of_round(int fd, const std::vector<std::string> &params);

    /**
     * Function that send the "GAME_OVER" message to the client
     * S -> C : GAME_OVER|<WIN/LOSS/DRAW>
     * C -> S : OK
     * @param fd File descriptor of the socket
     */
    void game_over(int fd);

public:
    /**
     * Constructor of the class
     * Creates the socket and binds it to the port
     * @param port Port to bind the socket to
     */
    explicit Server(int port);

    /**
     * Destructor of the class
     * Closes the socket
     */
    ~Server();

    /**
     * Function that starts the server
     * Uses select to handle multiple clients
     */
    void run();
};
