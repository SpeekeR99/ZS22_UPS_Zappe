#include "Game.h"

Game::Game(uint32_t id, const std::shared_ptr<Player>& player) : id(id), state(G_S_WAITING_FOR_PLAYERS), is_new_round(false), paused(false), game_over(false) {
    player1 = player;
    player1->state = P_S_IN_LOBBY;
    player1->score = 0;
}

void Game::join_game(const std::shared_ptr<Player>& player) {
    player2 = player;
    player2->game = shared_from_this();
    player1->state = P_S_IN_GAME;
    player2->state = P_S_IN_GAME;
    player2->score = 0;

    state = G_S_PLAYING;
    start_round();
}

std::shared_ptr<Player> Game::get_opponent(const std::shared_ptr<Player> &player) const {
    if (player1->name == player->name) return player2;
    else if (player2->name == player->name) return player1;
    else return nullptr;
}

void Game::start_round() {
    player1->randomize_hand();
    player2->randomize_hand();

    player1->can_play = true;
    player2->can_play = true;
    is_new_round = true;
}

bool Game::check_if_all_players_played() {
    return !player1->can_play && !player2->can_play;
}

void Game::end_round() {
    auto score1 = player1->evaluate_hand();
    auto score2 = player2->evaluate_hand();
    if (score1 > score2) player1->score++;
    else if (score1 < score2) player2->score++;
    else { // draw
        player1->score++;
        player2->score++;
    }
    if (player1->score == BEST_OF || player2->score == BEST_OF)
        end_game();
    else
        start_round();
}

void Game::end_game() {
    game_over = true;
    state = G_S_FINISHED;
}
