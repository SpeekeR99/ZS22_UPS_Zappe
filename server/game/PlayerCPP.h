#pragma once

#include <string>
#include <memory>
#include "GameCPP.h"

constexpr unsigned char PLAYER_STATES_COUNT = 6;
constexpr unsigned char PLAYER_EVENTS_COUNT = 5;
constexpr unsigned char PLAYER_MAX_NAME_LEN = 31;
constexpr unsigned char NUMBER_OF_DICE = 5;

enum class PlayerState {
    P_S_NOT_ALLOWED = 0,
    P_S_INIT = 1,
    P_S_WAITING = 2,
    P_S_PLAYING = 3,
    P_S_PAUSED = 4,
    P_S_FINISHED = 5
};

enum class PlayerEvent {
    P_E_WAIT = 0,
    P_E_PLAY = 1,
    P_E_PAUSE = 2,
    P_E_FINISH = 3
};

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
