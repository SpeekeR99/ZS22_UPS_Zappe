#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include <stdlib.h>
#include "typedefs.h"
#include "player.h"

#define GAME_STATES_COUNT 6
#define GAME_EVENTS_COUNT 4

typedef enum the_game_state {
    G_S_NOT_ALLOWED = 0,
    G_S_INIT = 1,
    G_S_WAITING = 2,
    G_S_PLAYING = 3,
    G_S_PAUSED = 4,
    G_S_FINISHED = 5
} game_state;

typedef enum the_game_event {
    G_E_WAIT = 0,
    G_E_PLAY = 1,
    G_E_PAUSE = 2,
    G_E_FINISH = 3
} game_event;

typedef struct the_game {
    uint id;
    struct the_player *player1;
    struct the_player *player2;
    game_state state;
} game;

game *init_game(uint id);
void join_game(game *game, struct the_player *player);
void start_game(game *game);
void game_loop(game *game);
void end_of_turn(game *game);
void end_game(game *game);
void free_game(game **game);

#endif
