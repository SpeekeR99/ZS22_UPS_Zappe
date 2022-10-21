#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include <string.h>
#include "typedefs.h"
#include "game.h"
#include "random.h"

#define PLAYER_STATES_COUNT 6
#define PLAYER_EVENTS_COUNT 5
#define MAX_NAME_LEN 31
#define NUM_OF_DICE 5

typedef enum the_player_state {
    P_S_NOT_ALLOWED = 0,
    P_S_INIT = 1,
    P_S_WAITING = 2,
    P_S_IN_GAME = 3,
    P_S_IN_GAME_ON_TURN = 4,
    P_S_DISCONNECTED = 5
} player_state;

typedef enum the_player_event {
    P_E_WAIT = 0,
    P_E_PLAY = 1,
    P_E_YOUR_TURN = 2,
    P_E_PLAYED_TURN = 3,
    P_E_DISCONNECT = 4
} player_event;

typedef struct the_player {
    int socket;
    char name[MAX_NAME_LEN];
    struct the_game *game;
    byte hand[NUM_OF_DICE];
    uint score;
    player_state state;
} player;

player *init_player(int socket, const char *name);
void randomize_hand(player *player);
int reroll_hand(player *player, const int *indices, int indices_len);
int evaluate_hand(player *player);
void free_player(player **player);

#endif
