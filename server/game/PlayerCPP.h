#pragma once

#include <string>
#include <memory>
#include <map>
#include "StateMachine.h"
#include "GameCPP.h"

constexpr unsigned char PLAYER_MAX_NAME_LEN = 31;
constexpr unsigned char NUMBER_OF_DICE = 5;

class PlayerCPP{
private:
    int socket;
    std::string name;
    std::shared_ptr<GameCPP> game;
    std::array<int, NUMBER_OF_DICE> hand;
    unsigned int score;
    PlayerState state;

public:
    PlayerCPP(int socket, std::string name);
    ~PlayerCPP();

    void randomize_hand();
    int reroll_hand(const std::array<int, NUMBER_OF_DICE> &indices);
    int evaluate_hand();
};
