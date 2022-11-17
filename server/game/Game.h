#pragma once

#include <memory>
#include <map>
#include "StateMachine.h"
#include "Player.h"

constexpr unsigned char BEST_OF = 3;

class Player;

class Game : public std::enable_shared_from_this<Game> {
public:
    uint32_t id;
    std::shared_ptr<Player> player1;
    std::shared_ptr<Player> player2;
    GameState state;
    bool is_new_round;
    bool paused;
    bool game_over;

    Game(uint32_t id, const std::shared_ptr<Player>& player);
    ~Game() = default;

    void join_game(const std::shared_ptr<Player>& player);
    std::shared_ptr<Player> get_opponent(const std::shared_ptr<Player>& player) const;
    void start_round();
    bool check_if_all_players_played();
    void end_round();
    void end_game();
};
