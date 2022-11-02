#include "StateMachine.h"

StateMachine::GameTransitionMap StateMachine::game_transitions = {
        {{G_S_WAITING_FOR_PLAYERS, G_E_PLAY},      G_S_PLAYING},
        {{G_S_PLAYING,             G_E_PAUSE},     G_S_PAUSED},
        {{G_S_PLAYING,             G_E_FINISH},    G_S_FINISHED},
        {{G_S_PAUSED,              G_E_RECONNECT}, G_S_PLAYING},
        {{G_S_PAUSED,              G_E_FINISH},    G_S_FINISHED},
        {{G_S_FINISHED,            G_E_PLAY},      G_S_WAITING_FOR_PLAYERS}
};

StateMachine::PlayerTransitionMap StateMachine::player_transitions = {
        {{P_S_IN_MAIN_MENU, P_E_JOIN_LOBBY}, P_S_IN_LOBBY},
        {{P_S_IN_LOBBY,     P_E_CANCEL},     P_S_IN_MAIN_MENU},
        {{P_S_IN_LOBBY,     P_E_PLAY},       P_S_IN_GAME},
        {{P_S_IN_GAME,      P_E_PLAY_AGAIN}, P_S_IN_LOBBY},
        {{P_S_IN_GAME,      P_E_DISCONNECT}, P_S_DISCONNECTED},
        {{P_S_IN_GAME,      P_E_CANCEL},     P_S_IN_MAIN_MENU},
        {{P_S_DISCONNECTED, P_E_RECONNECT},  P_S_IN_GAME}
};

bool StateMachine::is_transition_possible(GameState state, GameEvent event) {
    return StateMachine::game_transitions.count({state, event});
}

bool StateMachine::is_transition_possible(PlayerState state, PlayerEvent event) {
    return StateMachine::player_transitions.count({state, event});
}

GameState StateMachine::transition(GameState current_state, GameEvent event) {
    if (is_transition_possible(current_state, event))
        return StateMachine::game_transitions.at({current_state, event});
    return current_state;
}

PlayerState StateMachine::transition(PlayerState current_state, PlayerEvent event) {
    if (is_transition_possible(current_state, event))
        return StateMachine::player_transitions.at({current_state, event});
    return current_state;
}
