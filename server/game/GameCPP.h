#pragma once

#include <memory>
#include <map>
#include "StateMachine.h"
#include "PlayerCPP.h"

class PlayerCPP;

class GameCPP {
public:
    std::shared_ptr<PlayerCPP> player1;
    std::shared_ptr<PlayerCPP> player2;
    GameState state;

    GameCPP();
    ~GameCPP() = default;

    void join_game(const std::shared_ptr<PlayerCPP>& player);
    void start_round();
    void end_round();
    void end_game();
};
