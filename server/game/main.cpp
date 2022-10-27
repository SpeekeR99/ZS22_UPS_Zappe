#include <iostream>
#include "StateMachine.h"
#include "GameCPP.h"
#include "PlayerCPP.h"

int main() {
    std::shared_ptr<GameCPP> game = std::make_shared<GameCPP>();
    std::shared_ptr<PlayerCPP> player1 = std::make_shared<PlayerCPP>(1, "Player 1");
    std::shared_ptr<PlayerCPP> player2 = std::make_shared<PlayerCPP>(2, "Player 2");

    std::cout << "Game state: " << game->state << std::endl;
    std::cout << "Player 1 state: " << player1->state << std::endl;
    std::cout << "Player 2 state: " << player2->state << std::endl;

    game->join_game(player1);

    std::cout << "Game state: " << game->state << std::endl;
    std::cout << "Player 1 state: " << player1->state << std::endl;
    std::cout << "Player 2 state: " << player2->state << std::endl;

    game->join_game(player2);

    std::cout << "Game state: " << game->state << std::endl;
    std::cout << "Player 1 state: " << player1->state << std::endl;
    std::cout << "Player 2 state: " << player2->state << std::endl;

    return EXIT_SUCCESS;
}
