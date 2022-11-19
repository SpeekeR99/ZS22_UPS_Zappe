#include "Player.h"

Player::Player(int socket) : socket{socket}, hand{} {
    name = "";
    game = nullptr;
    score = 0;
    state = P_S_IN_MAIN_MENU;
    random = std::make_shared<MyRandom>();
    can_play = false;
    handshake = false;
    logged_in = false;
    accepted_end_of_round = false;
    number_of_error_messages = 0;
}

void Player::randomize_hand() {
    for (auto &i: hand)
        i = random->roll_a_die();
}

void Player::reroll_hand(const std::array<int, NUMBER_OF_DICE> &indices) {
    for (int i = 0; i < indices.size(); i++)
        if (indices[i]) hand[i] = random->roll_a_die();

    can_play = false;

    // Check if both players played
    if (game->check_if_all_players_played())
        game->end_round();
}

void set_score(int &points, int &score_num, int score, int which_1, int which_2) {
    points = score;
    score_num = 100 * score + which_1 * 10 + which_2;
}

int Player::evaluate_hand() {
    std::array<int, 6> counts = {0, 0, 0, 0, 0, 0};
    int points = 0;
    int score_num = 0;

    for (int i = 0; i < NUMBER_OF_DICE; i++) {
        counts[hand[i] - 1]++;
    }

    for (int i = 0; i < MAX_DIE_NUM; i++) {
        if (counts[i] == 5 && points < 8) // Five of a kind
            set_score(points, score_num, 8, i + 1, i + 1);
        if (counts[i] == 4 && points < 7) // Four of a kind
            set_score(points, score_num, 7, i + 1, i + 1);
        if (counts[i] == 3) {
            for (int j = 0; j < MAX_DIE_NUM; j++) {
                if (counts[j] == 2 && i != j && points < 6) // Full house
                    set_score(points, score_num, 6, i + 1, j + 1);
            }
            if (points < 3) // Three of a kind
                set_score(points, score_num, 3, i + 1, i + 1);
        }
        if (counts[i] == 2) {
            for (int j = 0; j < MAX_DIE_NUM; j++) {
                if (counts[j] == 2 && i != j && points < 2) // Two pairs
                    set_score(points, score_num, 2, i + 1, j + 1);
            }
            if (points < 1) // One pair
                set_score(points, score_num, 1, i + 1, i + 1);
        } else {
            if (counts[0] == 1 && counts[1] == 1 && counts[2] == 1 && counts[3] == 1 && counts[4] == 1 &&
                points < 4) // Small straight
                set_score(points, score_num, 4, 1, 5);
            else if (counts[1] == 1 && counts[2] == 1 && counts[3] == 1 && counts[4] == 1 && counts[5] == 1 &&
                     points < 5) // Big straight
                set_score(points, score_num, 5, 2, 6);
            else if (points == 0) // Nothing
                set_score(points, score_num, 0, 6, 6);
        }
    }
    return score_num;
}
