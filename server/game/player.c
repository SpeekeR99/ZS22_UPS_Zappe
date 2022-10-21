#include "player.h"

const char * const player_state_str[] = {
        [P_S_NOT_ALLOWED] = "not allowed",
        [P_S_INIT] = "init",
        [P_S_WAITING] = "waiting",
        [P_S_IN_GAME]  = "in game",
        [P_S_DISCONNECTED]  = "disconnected"
};

const char * const player_event_str[] = {
        [P_E_WAIT] = "wait",
        [P_E_PLAY] = "play",
        [P_E_YOUR_TURN] = "your turn",
        [P_E_PLAYED_TURN] = "not your turn",
        [P_E_DISCONNECT] = "disconnect",
};

player_state player_transitions[PLAYER_STATES_COUNT][PLAYER_EVENTS_COUNT] = {
        [P_S_INIT][P_E_WAIT] = P_S_WAITING,
        [P_S_WAITING][P_E_PLAY] = P_S_IN_GAME,
        [P_S_IN_GAME][P_E_YOUR_TURN] = P_S_IN_GAME_ON_TURN,
        [P_S_IN_GAME_ON_TURN][P_E_PLAYED_TURN] = P_S_IN_GAME,
        [P_S_IN_GAME][P_E_DISCONNECT] = P_S_DISCONNECTED,
        [P_S_IN_GAME_ON_TURN][P_E_DISCONNECT] = P_S_DISCONNECTED,
        [P_S_DISCONNECTED][P_E_PLAY] = P_S_IN_GAME
};

player *init_player(int socket, const char *name) {
    if (strlen(name) > MAX_NAME_LEN) return NULL;
    player *temp = calloc(1, sizeof(player));
    if (!temp) return NULL;
    temp->socket = socket;
    for (int i = 0; i < strlen(name); i++) {
        temp->name[i] = name[i];
    }
    temp->state = P_S_INIT;
    return temp;
}

int reroll_hand(player *player, const int *indices, int indices_len) {
    for (int i = 0; i < indices_len; i++) {
        if (indices[i] < 0 || indices[i] >= NUM_OF_DICE) return FAILURE;
        player->hand[indices[i]] = roll_a_die();
    }
    return SUCCESS;
}

int evaluate_hand(player *player) {
    //
}

void free_player(player **player) {
    free(*player);
    *player = NULL;
}
