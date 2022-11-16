#pragma once

enum GameState {
    G_S_WAITING_FOR_PLAYERS,
    G_S_PLAYING,
    G_S_PAUSED,
    G_S_FINISHED
};

enum PlayerState {
    P_S_IN_MAIN_MENU,
    P_S_IN_LOBBY,
    P_S_IN_GAME,
    P_S_DISCONNECTED
};
