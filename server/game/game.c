#include "game.h"

game *init_game(uint id, player *player1, player *player2) {
    game *temp = (game *) calloc(1, sizeof(game));
    temp->id = id;
    temp->player1 = player1;
    temp->player2 = player2;
    temp->state = 0;
    return temp;
}

void start_game(game *game) {
    game->state = 1;
}

void end_game(game *game) {
    game->state = 2;
}

int evaluate_hand(player *player) {
    //
}

int reroll_hand(player *player, const int *indices) {
    //
}

int free_game(game **game) {
    free(*game);
    *game = NULL;
    return 1;
}