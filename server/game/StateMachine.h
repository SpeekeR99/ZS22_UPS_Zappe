#pragma once

/**
 * Possible states of the game
 * Game is in G_S_WAITING_FOR_PLAYERS state when it is created and waiting for the other player to join
 * Game is in G_S_PLAYING state when the game is in progress
 * Game is in G_S_FINISHED state when the game is finished
 */
enum GameState {
    G_S_WAITING_FOR_PLAYERS,
    G_S_PLAYING,
    G_S_FINISHED
};

/**
 * Possible states of the player
 * Player is in P_S_IN_MAIN_MENU state when the player is in the main menu
 * Player is in P_S_IN_LOBBY state when the player created the game and the game is in G_S_WAITING_FOR_PLAYERS state
 * Player is in P_S_IN_GAME state when the player is in the game
 * Player is in P_S_DISCONNECTED state when the player is disconnected (not intentionally)
 */
enum PlayerState {
    P_S_IN_MAIN_MENU,
    P_S_IN_LOBBY,
    P_S_IN_GAME,
    P_S_DISCONNECTED
};
