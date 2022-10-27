#pragma once

#include <map>

enum GameState {
    G_S_WAITING,
    G_S_PLAYING_START_OF_ROUND,
    G_S_PLAYING_END_OF_ROUND,
    G_S_PAUSED,
    G_S_FINISHED
};

enum GameEvent {
    G_E_PLAY,
    G_E_START_ROUND,
    G_E_END_ROUND,
    G_E_PAUSE,
    G_E_RECONNECT,
    G_E_FINISH,
};

enum PlayerState {
    P_S_IN_MAIN_MENU,
    P_S_IN_LOBBY,
    P_S_IN_GAME_CAN_PLAY,
    P_S_IN_GAME_CANNOT_PLAY,
    P_S_DISCONNECTED
};

enum PlayerEvent {
    P_E_JOIN_LOBBY,
    P_E_CANCEL,
    P_E_PLAY,
    P_E_YOUR_TURN,
    P_E_DISCONNECT,
    P_E_RECONNECT,
    P_E_PLAY_AGAIN
};

class StateMachine {
public:
    typedef std::map<std::pair<GameState, GameEvent>, GameState> GameTransitionMap;
    typedef std::map<std::pair<PlayerState, PlayerEvent>, PlayerState> PlayerTransitionMap;
    static GameTransitionMap game_transitions;
    static PlayerTransitionMap player_transitions;

    static bool is_transition_possible(GameState state, GameEvent event);
    static bool is_transition_possible(PlayerState state, PlayerEvent event);
    static GameState transition(GameState current_state, GameEvent event);
    static PlayerState transition(PlayerState current_state, PlayerEvent event);
};
