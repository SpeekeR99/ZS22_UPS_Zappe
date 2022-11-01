#pragma once

#include <utility>
#include <string>
#include <memory>
#include <map>
#include "StateMachine.h"
#include "Game.h"
#include "MyRandom.h"

constexpr unsigned char PLAYER_MAX_NAME_LEN = 31;
constexpr unsigned char NUMBER_OF_DICE = 5;

class Game;

class Player {
public:
    int socket;
    std::string name;
    std::shared_ptr<Game> game;
    std::array<int, NUMBER_OF_DICE> hand{};
    unsigned int score{};
    PlayerState state;
    std::shared_ptr<MyRandom> random;

    explicit Player(int socket);
    ~Player() = default;

    void randomize_hand();
    void reroll_hand(const std::array<int, NUMBER_OF_DICE> &indices);
    int evaluate_hand();
};
