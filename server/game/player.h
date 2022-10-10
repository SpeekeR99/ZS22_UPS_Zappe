#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include "game.h"

#define MAX_NAME_LEN 31
#define NUM_OF_DICE 5

typedef unsigned int uint;
typedef unsigned char byte;

typedef struct the_player {
    uint id;
    char name[MAX_NAME_LEN];
    game *game;
    int hand[NUM_OF_DICE];
    byte score;
    int state;
} player;

#endif
