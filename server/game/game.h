#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include <stdlib.h>
#include "player.h"

typedef unsigned int uint;

typedef struct the_game {
    uint id;
    player *player1;
    player *player2;
    int state;
} game;

game *init_game(uint id, player *player1, player *player2);
void start_game(game *game);
void end_game(game *game);
int evaluate_hand(player *player);
int reroll_hand(player *player, const int *indices);
int free_game(game **game);

#endif
