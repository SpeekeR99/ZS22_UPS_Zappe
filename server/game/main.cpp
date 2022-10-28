#include <iostream>
#include "StateMachine.h"
#include "Game.h"
#include "Player.h"

int main() {
    std::shared_ptr<MyRandom> random = std::make_shared<MyRandom>();
    std::shared_ptr<Game> game = std::make_shared<Game>();
    std::shared_ptr<Player> player1 = std::make_shared<Player>(1, "Player 1");
    std::shared_ptr<Player> player2 = std::make_shared<Player>(2, "Player 2");
    player1->random = random;
    player2->random = random;

    std::cout << "Game state: " << game->state << std::endl;
    std::cout << "Player 1 state: " << player1->state << std::endl;
    std::cout << "Player 2 state: " << player2->state << std::endl;

    game->join_game(player1);

    std::cout << "Game state: " << game->state << std::endl;
    std::cout << "Player 1 state: " << player1->state << std::endl;
    std::cout << "Player 2 state: " << player2->state << std::endl;

    game->join_game(player2);

    std::cout << "Game state: " << game->state << std::endl;
    std::cout << "Player 1 state: " << game->player1->state << std::endl;
    std::cout << "Player 2 state: " << game->player2->state << std::endl;

    std::cout << "Player 1 hand: ";
    for (auto &i : player1->hand) std::cout << i << " ";
    std::cout << std::endl;
    std::cout << "Player 2 hand: ";
    for (auto &i : player2->hand) std::cout << i << " ";
    std::cout << std::endl;

    player1->reroll_hand({0, 1, 1, 1, 0});
    std::cout << "Player 1 hand: ";
    for (auto &i : player1->hand) std::cout << i << " ";
    std::cout << std::endl;

    std::cout << "Game state: " << game->state << std::endl;
    std::cout << "Player 1 state: " << player1->state << std::endl;
    std::cout << "Player 2 state: " << player2->state << std::endl;

    player1->reroll_hand({0, 1, 1, 1, 0});
    player1->reroll_hand({0, 1, 1, 1, 0});

    std::cout << "Player 1 hand: ";
    for (auto &i : player1->hand) std::cout << i << " ";
    std::cout << std::endl;

    std::cout << "Game state: " << game->state << std::endl;
    std::cout << "Player 1 state: " << player1->state << std::endl;
    std::cout << "Player 2 state: " << player2->state << std::endl;

    player2->reroll_hand({0, 1, 0, 1, 1});

    std::cout << "Game state: " << game->state << std::endl;
    std::cout << "Player 1 state: " << player1->state << std::endl;
    std::cout << "Player 2 state: " << player2->state << std::endl;

    std::cout << "Player 1 hand: ";
    for (auto &i : player1->hand) std::cout << i << " ";
    std::cout << std::endl;
    std::cout << "Player 2 hand: ";
    for (auto &i : player2->hand) std::cout << i << " ";
    std::cout << std::endl;

    std::cout << "Player 1 score: " << player1->score << std::endl;
    std::cout << "Player 2 score: " << player2->score << std::endl;

    return EXIT_SUCCESS;
}
