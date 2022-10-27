#pragma once

#include <memory>
#include <map>
#include "StateMachine.h"
#include "PlayerCPP.h"

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
