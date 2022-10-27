#pragma once

#include <memory>
#include <map>
#include "PlayerCPP.h"

constexpr unsigned char GAME_STATES_COUNT = 5;
constexpr unsigned char GAME_EVENTS_COUNT = 6;

enum GameState {
    G_S_WAITING,
    G_S_PLAYING_START_OF_ROUND,
    G_S_PLAYING_END_OF_ROUND,
    G_S_PAUSED,
    G_S_FINISHED
};

enum GameEvent {
    G_E_PLAY,
    G_E_START_ROUND,
    G_E_END_ROUND,
    G_E_PAUSE,
    G_E_RECONNECT,
    G_E_FINISH,
};

std::map<std::pair<GameState, GameEvent>, GameState> game_transitions = {
        {{G_S_WAITING, G_E_PLAY}, G_S_PLAYING_START_OF_ROUND},
        {{G_S_PLAYING_START_OF_ROUND, G_E_END_ROUND}, G_S_PLAYING_END_OF_ROUND},
        {{G_S_PLAYING_START_OF_ROUND, G_E_PAUSE}, G_S_PAUSED},
        {{G_S_PLAYING_END_OF_ROUND, G_E_START_ROUND}, G_S_PLAYING_START_OF_ROUND},
        {{G_S_PLAYING_END_OF_ROUND, G_E_PAUSE}, G_S_PAUSED},
        {{G_S_PLAYING_END_OF_ROUND, G_E_FINISH}, G_S_FINISHED},
        {{G_S_PAUSED, G_E_RECONNECT}, G_S_PLAYING_START_OF_ROUND},
        {{G_S_PAUSED, G_E_FINISH}, G_S_FINISHED},
        {{G_S_FINISHED, G_E_PLAY}, G_S_WAITING}
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
