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

game *init_game(uint id) {
    game *temp = (game *) calloc(1, sizeof(game));
    temp->id = id;
    temp->state = G_S_INIT;
    return temp;
}

void join_game(game *game, player *player) {
    if (game_transitions[game->state][G_E_WAIT] != G_S_NOT_ALLOWED)
        game->state = game_transitions[game->state][G_E_WAIT];
    else
        ;// uzivatel je debil

    if (!(game->player1)) game->player1 = player;
    else if (!(game->player2)) game->player2 = player;

    if (game->player1 && game->player2)
        start_game(game);
}

void start_game(game *game) {
    if (game_transitions[game->state][G_E_PLAY] != G_S_NOT_ALLOWED)
        game->state = game_transitions[game->state][G_E_PLAY];
}

void end_game(game *game) {
    if (game_transitions[game->state][G_E_FINISH] != G_S_NOT_ALLOWED)
        game->state = game_transitions[game->state][G_E_FINISH];
}

int free_game(game **game) {
    free(*game);
    *game = NULL;
    return 1;
}