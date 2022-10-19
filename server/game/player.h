#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include "typedefs.h"
#include "game.h"

#define PLAYER_STATES_COUNT 5
#define PLAYER_EVENTS_COUNT 3
#define MAX_NAME_LEN 31
#define NUM_OF_DICE 5

typedef enum the_player_state {
    P_S_NOT_ALLOWED = 0,
    P_S_INIT = 1,
    P_S_WAITING = 2,
    P_S_IN_GAME = 3,
    P_S_DISCONNECTED = 4
} player_state;

typedef enum the_player_event {
    P_E_WAIT = 0,
    P_E_PLAY = 1,
    P_E_DISCONNECT = 2
} player_event;

typedef struct the_player {
    int socket;
    char name[MAX_NAME_LEN];
    game *game;
    byte hand[NUM_OF_DICE];
    uint score;
    player_state state;
} player;

#endif
