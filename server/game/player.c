#include "player.h"

const char * const state_str[] = {
        [P_S_NOT_ALLOWED] = "not allowed",
        [P_S_INIT] = "init",
        [P_S_WAITING] = "waiting",
        [P_S_IN_GAME]  = "in game",
        [P_S_DISCONNECTED]  = "disconnected"
};

const char * const event_str[] = {
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
