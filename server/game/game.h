#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include <stdlib.h>
#include "typedefs.h"
#include "player.h"

#define GAME_STATES_COUNT 6
#define GAME_EVENTS_COUNT 4

typedef enum the_game_state {
    G_S_NOT_ALLOWED = 0,
    G_S_INNIT = 1,
    G_S_WAITING = 2,
    G_S_PLAYING = 3,
    G_S_PAUSE = 4,
    G_S_QUIT = 5
} game_state;

typedef enum the_game_event {
    G_E_WAIT = 0,
    G_E_PLAY = 1,
    G_E_PAUSE = 2,
    G_E_QUIT = 3
} game_event;

typedef struct the_game {
    uint id;
    player *player1;
    player *player2;
    game_state state;
} game;

game *init_game(uint id, player *player1, player *player2);
void start_game(game *game);
void end_game(game *game);
int evaluate_hand(player *player);
int reroll_hand(player *player, const int *indices);
int free_game(game **game);

#endif
