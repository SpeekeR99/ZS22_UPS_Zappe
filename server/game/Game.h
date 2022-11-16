#pragma once

#include <iostream>
#include <memory>
#include <map>
#include "StateMachine.h"
#include "Player.h"

class Player;

class Game : public std::enable_shared_from_this<Game> {
public:
    uint32_t id;
    std::shared_ptr<Player> player1;
    std::shared_ptr<Player> player2;
    GameState state;
    bool is_start_round;

    explicit Game(uint32_t id);
    ~Game() = default;

    void join_game(const std::shared_ptr<Player>& player);
    std::shared_ptr<Player> get_opponent(const std::shared_ptr<Player>& player) const;
    void start_round();
    bool check_if_all_players_played();
    void end_round();
    void end_game();
};
