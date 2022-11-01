#include <iostream>
#include "Player.h"

Player::Player(int socket) : socket{socket} {
    state = PlayerState::P_S_IN_MAIN_MENU;
}

void Player::randomize_hand() {
    for (auto &i : hand) i = random->roll_a_die();
}

void Player::reroll_hand(const std::array<int, NUMBER_OF_DICE> &indices) {
    if (state != PlayerState::P_S_IN_GAME_CAN_PLAY) return;

    for (int i = 0; i < indices.size(); i++) {
        if (indices[i] == 1) hand[i] = random->roll_a_die();
    }
    if (StateMachine::is_transition_possible(state, PlayerEvent::P_E_PLAY))
        state = StateMachine::transition(state, PlayerEvent::P_E_PLAY);

    if (game->check_if_all_players_played() && StateMachine::is_transition_possible(game->state, GameEvent::G_E_END_ROUND)) {
        game->state = StateMachine::transition(game->state, GameEvent::G_E_END_ROUND);
        game->end_round();
    }
}

void set_score(int &points, std::array<int, 2> &which_cards, std::string &what_combination, int score, int which_1, int which_2, const std::string &combination) {
    points = score;
    which_cards[0] = which_1;
    which_cards[1] = which_2;
    what_combination = combination;
}

int Player::evaluate_hand() {
    std::string what_combination;
    std::array<int, 2> which_cards{-1, -1};
    std::array<int, 6> counts = {0, 0, 0, 0, 0, 0};
    int points = 0;

    for (int i = 0; i < NUMBER_OF_DICE; i++) {
        counts[hand[i] - 1]++;
    }

    for (int i = 0; i < MAX_DIE_NUM; i++) {
        if (counts[i] == 5 && points < 8) {
            set_score(points, which_cards, what_combination, 8, i + 1, i + 1, "Five of a kind");
        }
        if (counts[i] == 4 && points < 7) {
            set_score(points, which_cards, what_combination, 7, i + 1, i + 1, "Four of a kind");
        }
        if (counts[i] == 3) {
            for (int j = 0; j < MAX_DIE_NUM; j++) {
                if (counts[j] == 2 && i != j && points < 6) {
                    set_score(points, which_cards, what_combination, 6, i + 1, j + 1, "Full house");
                }
            }
            if (points < 3) {
                set_score(points, which_cards, what_combination, 3, i + 1, i + 1, "Three of a kind");
            }
        }
        if (counts[i] == 2) {
            for (int j = 0; j < MAX_DIE_NUM; j++) {
                if (counts[j] == 2 && i != j && points < 5) {
                    set_score(points, which_cards, what_combination, 5, i + 1, j + 1, "Two pairs");
                }
            }
            if (points < 2) {
                set_score(points, which_cards, what_combination, 2, i + 1, i + 1, "One pair");
            }
        }
        else {
            if (counts[0] == 1 && counts[1] == 1 && counts[2] == 1 && counts[3] == 1 && counts[4] == 1 && points < 4) {
                set_score(points, which_cards, what_combination, 4, 1, 5, "Small straight");
            }
            else if (counts[1] == 1 && counts[2] == 1 && counts[3] == 1 && counts[4] == 1 && counts[5] == 1 && points < 4) {
                set_score(points, which_cards, what_combination, 5, 2, 6, "Big straight");
            }
            else if (points == 0) {
                set_score(points, which_cards, what_combination, 1, 6, 6, "Nothing");
            }
        }
    }
    return points;
}
