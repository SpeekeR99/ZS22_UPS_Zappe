#include "game.h"

const char * const game_state_str[] = {
        [G_S_NOT_ALLOWED] = "not allowed",
        [G_S_INIT] = "init",
        [G_S_WAITING] = "waiting",
        [G_S_PLAYING]  = "playing",
        [G_S_PAUSED]  = "paused",
        [G_S_FINISHED] = "finished"
};

const char * const game_event_str[] = {
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
    if (!temp) return NULL;
    temp->id = id;
    temp->state = G_S_INIT;
    return temp;
}

void join_game(game *game, struct the_player *player) {
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
    if (game_transitions[game->state][G_E_PLAY] != G_S_NOT_ALLOWED) {
        game->state = game_transitions[game->state][G_E_PLAY];
        start_round(game);
    }
}

void start_round(game *game) {
    // init players
}

void end_round(game *game) {
    // evaluate
    // end / start new round
}

void end_of_turn(game *game) {
    int score1 = evaluate_hand(game->player1);
    int score2 = evaluate_hand(game->player2);
    if (score1 > score2)
        game->player1->score++;
    else if (score1 < score2)
        game->player2->score++;
    else { // draw
        game->player1->score++;
        game->player2->score++;
    }
}

void end_game(game *game) {
    if (game_transitions[game->state][G_E_FINISH] != G_S_NOT_ALLOWED)
        game->state = game_transitions[game->state][G_E_FINISH];
}

void free_game(game **game) {
    free(*game);
    *game = NULL;
}