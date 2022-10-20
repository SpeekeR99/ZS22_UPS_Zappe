#include "game.h"

const char * const state_str[] = {
        [G_S_NOT_ALLOWED] = "not allowed",
        [G_S_INIT] = "init",
        [G_S_WAITING] = "waiting",
        [G_S_PLAYING]  = "playing",
        [G_S_PAUSED]  = "paused",
        [G_S_FINISHED] = "finished"
};

const char * const event_str[] = {
        [G_E_WAIT] = "wait",
        [G_E_PLAY] = "play",
        [G_E_PAUSE] = "pause",
        [G_E_FINISH] = "finish",
};

game_state game_transitions[GAME_STATES_COUNT][GAME_EVENTS_COUNT] = {
        [G_S_INIT][G_E_WAIT] = G_S_WAITING,
        [G_S_WAITING][G_E_PLAY] = G_S_PLAYING,
        [G_S_PLAYING][G_E_PAUSE] = G_S_PAUSED,
        [G_S_PAUSED][G_E_PLAY] = G_S_PLAYING,
        [G_S_PLAYING][G_E_FINISH] = G_S_FINISHED,
        [G_S_PAUSED][G_E_FINISH] = G_S_FINISHED
};

game *init_game(uint id, player *player1, player *player2) {
    game *temp = (game *) calloc(1, sizeof(game));
    temp->id = id;
    temp->player1 = player1;
    temp->player2 = player2;
    temp->state = G_S_INIT;
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