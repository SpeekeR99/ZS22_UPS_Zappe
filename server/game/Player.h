#pragma once

#include <utility>
#include <string>
#include <memory>
#include <map>
#include <chrono>
#include "StateMachine.h"
#include "Game.h"
#include "MyRandom.h"

/** Number of dice in a hand */
constexpr unsigned char NUMBER_OF_DICE = 5;

/** Forward declaration of Game class */
class Game;

/**
 * Class representing a player in the game
 */
class Player {
public:
    /** Player's file descriptor of the socket */
    int socket;
    /** Player's chosen nickname */
    std::string name;
    /** Player's game that he is currently in */
    std::shared_ptr<Game> game;
    /** Player's hand of dice */
    std::array<int, NUMBER_OF_DICE> hand;
    /** Player's score (winning points) */
    uint32_t score;
    /** Player's current state */
    PlayerState state;
    /** Player's random number generator for rolling the dice */
    std::shared_ptr<MyRandom> random;
    /** Player's bit flag for the possibility of rolling the dice in the round */
    bool can_play;
    /** Player's bit flag for the knowledge of the handshake being done */
    bool handshake;
    /** Player's bit flag for the knowledge of the login being successful */
    bool logged_in;
    /** Player's bit flag for the knowledge of the acceptance of the end of the round */
    bool accepted_end_of_round;
    /** Player's counter for the number of error messages sent */
    int number_of_error_messages;
    /** Time of the unintentional disconnection of the player */
    std::chrono::time_point<std::chrono::high_resolution_clock> disconnect_time;

    /**
     * Constructor of the Player class
     * @param socket File descriptor of the socket
     */
    explicit Player(int socket);

    /**
     * Destructor of the Player class
     */
    ~Player() = default;

    /**
     * Randomizes the player's hand of dice
     */
    void randomize_hand();

    /**
     * Rerolls the player's hand of dice according to the given bit flag array
     * @param indices Flag array of the dice to be rerolled
     */
    void reroll_hand(const std::array<int, NUMBER_OF_DICE> &indices);

    /**
     * Evaluates the player's hand of dice and returns the score value
     * Formula for evaluation is: COMBINATION * 100 + DIE_IN_COMBINATION * 10 + OTHER_DIE_IN_COMBINATION
     * Possible COMBINATION values are:
     * 8 - five of a kind
     * 7 - four of a kind
     * 6 - full house
     * 5 - big straight
     * 4 - small straight
     * 3 - three of a kind
     * 2 - two pairs
     * 1 - one pair
     * 0 - no combination
     * Examples: 866 - five sixes (highest possible score) (66666)
     *           651 - full house of fives and ones (55111)
     *           562 - big straight from two to six (23456)
     *           243 - two pairs of fours and threes (4433x)
     *           066 - no combination (12346) or (12356) or (12456) or (13456) (no combination always includes 6)
     * @return Score value of the player's hand of dice
     */
    int evaluate_hand();
};
