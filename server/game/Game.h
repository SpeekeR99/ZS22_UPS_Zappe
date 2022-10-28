#pragma once

#include <memory>
#include <map>
#include "StateMachine.h"
#include "Player.h"

class Player;

class Game : public std::enable_shared_from_this<Game> {
public:
    std::shared_ptr<Player> player1;
    std::shared_ptr<Player> player2;
    GameState state;

    Game();
    ~Game() = default;

    void join_game(const std::shared_ptr<Player>& player);
    void start_round();
    bool check_if_all_players_played();
    void end_round();
    void end_game();
};
