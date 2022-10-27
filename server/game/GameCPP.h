#pragma once

#include <memory>
#include <map>
#include "PlayerCPP.h"

constexpr unsigned char GAME_STATES_COUNT = 6;
constexpr unsigned char GAME_EVENTS_COUNT = 5;

enum GameState {
    G_S_NOT_ALLOWED = 0,
    G_S_INIT = 1,
    G_S_WAITING = 2,
    G_S_PLAYING = 3,
    G_S_PAUSED = 4,
    G_S_FINISHED = 5
};

enum GameEvent {
    G_E_WAIT = 0,
    G_E_PLAY = 1,
    G_E_PAUSE = 2,
    G_E_FINISH = 3
};

std::map<std::pair<GameState, GameEvent>, GameState> game_transitions = {
        {{G_S_INIT, G_E_WAIT}, G_S_WAITING}
};

class GameCPP{
private:
    std::shared_ptr<PlayerCPP> player1;
    std::shared_ptr<PlayerCPP> player2;
    GameState state;

public:
    GameCPP();
    ~GameCPP();

    void join_game(std::shared_ptr<PlayerCPP> player);
    void start_game();
    void start_round();
    void end_round();
    void end_game();
};
