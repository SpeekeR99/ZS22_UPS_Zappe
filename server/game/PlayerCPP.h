#pragma once

#include <string>
#include <memory>
#include "GameCPP.h"

constexpr unsigned char PLAYER_STATES_COUNT = 5;
constexpr unsigned char PLAYER_EVENTS_COUNT = 6;
constexpr unsigned char PLAYER_MAX_NAME_LEN = 31;
constexpr unsigned char NUMBER_OF_DICE = 5;

enum PlayerState {
    P_S_IN_MAIN_MENU,
    P_S_IN_LOBBY,
    P_S_IN_GAME_CAN_PLAY,
    P_S_IN_GAME_CANNOT_PLAY,
    P_S_DISCONNECTED
};

enum PlayerEvent {
    P_E_JOIN_LOBBY,
    P_E_CANCEL,
    P_E_PLAY,
    P_E_YOUR_TURN,
    P_E_DISCONNECT,
    P_E_RECONNECT,
    P_E_PLAY_AGAIN
};

std::map<std::pair<PlayerState, PlayerEvent>, PlayerState> player_transitions = {
        {{P_S_IN_MAIN_MENU, P_E_JOIN_LOBBY}, P_S_IN_LOBBY},
        {{P_S_IN_LOBBY, P_E_CANCEL}, P_S_IN_MAIN_MENU},
        {{P_S_IN_LOBBY, P_E_PLAY}, P_S_IN_GAME_CAN_PLAY},
        {{P_S_IN_GAME_CAN_PLAY, P_E_PLAY}, P_S_IN_GAME_CANNOT_PLAY},
        {{P_S_IN_GAME_CAN_PLAY, P_E_DISCONNECT}, P_S_DISCONNECTED},
        {{P_S_IN_GAME_CAN_PLAY, P_E_CANCEL}, P_S_IN_MAIN_MENU},
        {{P_S_IN_GAME_CANNOT_PLAY, P_E_YOUR_TURN}, P_S_IN_GAME_CAN_PLAY},
        {{P_S_IN_GAME_CANNOT_PLAY, P_E_DISCONNECT}, P_S_DISCONNECTED},
        {{P_S_IN_GAME_CANNOT_PLAY, P_E_CANCEL}, P_S_IN_MAIN_MENU},
        {{P_S_IN_GAME_CANNOT_PLAY, P_E_PLAY_AGAIN}, P_S_IN_LOBBY},
        {{P_S_DISCONNECTED, P_E_RECONNECT}, P_S_IN_GAME_CAN_PLAY}
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
