#pragma once

#include <memory>
#include <map>
#include "StateMachine.h"
#include "Player.h"

/** Constant of how many winning points does a player need to win the game */
constexpr unsigned char BEST_OF = 3;

/** Forward declaration of the Player class */
class Player;

/**
 * Class representing a game between two players
 */
class Game : public std::enable_shared_from_this<Game> {
public:
    /** Game id (unique) */
    uint32_t id;
    /** Creator of the game */
    std::shared_ptr<Player> player1;
    /** Joined (second) player of the game */
    std::shared_ptr<Player> player2;
    /** Current state of the game */
    GameState state;
    /** Flag indicating if the game started a new round */
    bool is_new_round;
    /** Flag indicating if the game ended a round */
    bool is_end_round;
    /** Flag for sending (to players) the end of round information only once */
    bool send_end_of_round;
    /** Flag indicating if the game is paused (one of the players disconnected) */
    bool paused;
    /** Flag indicating if the game is over */
    bool game_over;

    /**
     * Constructor of the Game class
     * @param id Unique id of the game
     * @param player Player who created the game (player1)
     */
    Game(uint32_t id, const std::shared_ptr<Player> &player);

    /**
     * Destructor of the Game class (default)
     */
    ~Game() = default;

    /**
     * Joins the game with the second player
     * @param player Player who joined the game (player2)
     */
    void join_game(const std::shared_ptr<Player> &player);

    /**
     * Gets the opponent of the player
     * @param player Player whose opponent we want to get
     * @return Player who is the opponent of the player
     */
    std::shared_ptr<Player> get_opponent(const std::shared_ptr<Player> &player) const;

    /**
     * Starts a new round
     * Rerolls the hands of both players and sets the flags
     */
    void start_round();

    /**
     * Checks if both players rerolled their hands
     * Ends the round if both players rerolled their hands
     * @return True if both players rerolled their hands, false otherwise
     */
    bool check_if_all_players_played();

    /**
     * Ends the round
     * Evaluates the hands of both players and sets the score
     * Ends the game if one of the players won
     * Starts a new round if the game is not over
     * Sets the flags
     */
    void end_round();

    /**
     * Ends the game
     * Sets the flags
     */
    void end_game();
};
