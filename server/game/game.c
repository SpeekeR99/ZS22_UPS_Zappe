#include "game.h"

const char * const state_str[] = {
        [G_S_NOT_ALLOWED] = "not allowed",
        [G_S_INNIT] = "init",
        [G_S_WAITING] = "waiting",
        [G_S_PLAYING]  = "playing",
        [G_S_PAUSE]  = "paused",
        [G_S_QUIT] = "quitting"
};

const char * const event_str[] = {
        [G_E_WAIT] = "start waiting",
        [G_E_PLAY] = "start playing",
        [G_E_PAUSE] = "start pause",
        [G_E_QUIT] = "start quit",
};

game_state transitions[GAME_STATES_COUNT][GAME_EVENTS_COUNT] = {
        [G_S_INNIT][G_E_WAIT] = G_S_WAITING,
        [G_S_WAITING][G_E_PLAY] = G_S_PLAYING,
        [G_S_PLAYING][G_E_PAUSE] = G_S_PAUSE,
        [G_S_PAUSE][G_E_PLAY] = G_S_PLAYING,
        [G_S_PLAYING][G_E_QUIT] = G_S_QUIT,
        [G_S_PAUSE][G_E_QUIT] = G_S_QUIT
};

game *init_game(uint id, player *player1, player *player2) {
    game *temp = (game *) calloc(1, sizeof(game));
    temp->id = id;
    temp->player1 = player1;
    temp->player2 = player2;
    temp->state = G_S_INNIT;
    return temp;
}

void start_game(game *game) {
    if (transitions[game->state][G_E_PLAY] != G_S_NOT_ALLOWED) {
        game->state = transitions[game->state][G_E_PLAY];
    }
}

void end_game(game *game) {
    if (transitions[game->state][G_E_QUIT] != G_S_NOT_ALLOWED) {
        game->state = transitions[game->state][G_E_QUIT];
    }
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