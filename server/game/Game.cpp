#include "Game.h"

Game::Game() {
    state = GameState::G_S_WAITING_FOR_PLAYERS;
}

void Game::join_game(const std::shared_ptr<Player>& player) {
    if (state == GameState::G_S_WAITING_FOR_PLAYERS && StateMachine::is_transition_possible(player->state, PlayerEvent::P_E_JOIN_LOBBY)) {
        if (!player1) player1 = player;
        else if (!player2) player2 = player;
        player->state = StateMachine::transition(player->state, PlayerEvent::P_E_JOIN_LOBBY);
        player->game = shared_from_this();
    }
    if (StateMachine::is_transition_possible(state, G_E_PLAY) && player1 && player2) {
        state = StateMachine::transition(state, G_E_PLAY);
        start_round();
    }
}

void Game::start_round() {
    player1->randomize_hand();
    player2->randomize_hand();
    if (StateMachine::is_transition_possible(player1->state, P_E_PLAY))
        player1->state = StateMachine::transition(player1->state, P_E_PLAY);
    if (StateMachine::is_transition_possible(player2->state, P_E_PLAY))
        player2->state = StateMachine::transition(player2->state, P_E_PLAY);
}

bool Game::check_if_all_players_played() {
    return player1->state == PlayerState::P_S_IN_GAME_CANNOT_PLAY && player2->state == PlayerState::P_S_IN_GAME_CANNOT_PLAY;
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
    if (StateMachine::is_transition_possible(state, G_E_FINISH) && (player1->score == 3 || player2->score == 3)) {
        state = StateMachine::transition(state, G_E_FINISH);
        end_game();
    } else if (StateMachine::is_transition_possible(state, G_E_START_ROUND)) {
        state = StateMachine::transition(state, G_E_START_ROUND);
        start_round();
    }
}

void Game::end_game() {
    // idk yet
}
