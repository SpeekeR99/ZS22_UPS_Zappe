#include "player.h"

const char * const state_str[] = {
        [P_S_NOT_ALLOWED] = "not allowed",
        [P_S_INIT] = "init",
        [P_S_WAITING] = "waiting",
        [P_S_IN_GAME]  = "in game",
        [P_S_DISCONNECTED]  = "disconnected"
};

const char * const event_str[] = {
        [P_E_WAIT] = "start waiting",
        [P_E_PLAY] = "start playing",
        [P_E_DISCONNECT] = "start disconnect",
};

player_state transitions[PLAYER_STATES_COUNT][PLAYER_EVENTS_COUNT] = {
        [P_S_INIT][P_E_WAIT] = P_S_WAITING,
        [P_S_WAITING][P_E_PLAY] = P_S_IN_GAME,
        [P_S_IN_GAME][P_E_DISCONNECT] = P_S_DISCONNECTED,
        [P_S_DISCONNECTED][P_E_PLAY] = P_S_IN_GAME
};
