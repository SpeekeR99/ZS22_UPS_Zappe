//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//#include <string.h>
#include <iostream>
#include "StateMachine.h"
#include "GameCPP.h"
#include "PlayerCPP.h"

//#define MAX_DIE_NUM 6
//#define MIN_DIE_NUM 1
//#define NUMBER_OF_DICE 5
//
//void print_array(int *array, int len) {
//    int i;
//    printf("[");
//    for(i = 0; i < len - 1; i++) {
//        printf("%d ", array[i]);
//    }
//    printf("%d]\n", array[len - 1]);
//}
//
//int roll_a_die() {
//    return (rand() % (MAX_DIE_NUM - MIN_DIE_NUM + 1)) + MIN_DIE_NUM;
//}
//
//void reroll_hand(int *hand, const int *indices, int indices_len) {
//    int i;
//    for(i = 0; i < indices_len; i++) {
//        hand[indices[i]] = roll_a_die();
//    }
//}
//
//void set_score(int *score_ptr, int *which_cards, char *what_combination, int score, int which_1, int which_2, char *combination) {
//    *score_ptr = score;
//    which_cards[0] = which_1;
//    which_cards[1] = which_2;
//    memset(what_combination, 0, 100);
//    strncpy(what_combination, combination, 99);
//}
//
//int evaluate_hand(const int *hand) {
//    char what_combination[100];
//    int i, j, score = 0, which_cards[2] = {0, 0}, counts[6] = {0, 0, 0, 0, 0, 0};
//
//    for (i = 0; i < NUMBER_OF_DICE; i++) {
//        counts[hand[i] - 1]++;
//    }
//
//    for (i = 0; i < MAX_DIE_NUM; i++) {
//        if (counts[i] == 5 && score < 8) {
//            set_score(&score, which_cards, what_combination, 8, i + 1, i + 1, "Five of a kind");
//        }
//        else if (counts[i] == 4 && score < 7) {
//            set_score(&score, which_cards, what_combination, 7, i + 1, i + 1, "Four of a kind");
//        }
//        else if (counts[i] == 3) {
//            for (j = 0; j < MAX_DIE_NUM; j++) {
//                if (counts[j] == 2 && i != j && score < 6) {
//                    set_score(&score, which_cards, what_combination, 6, i + 1, j + 1, "Full house");
//                }
//            }
//            if (score < 3) {
//                set_score(&score, which_cards, what_combination, 3, i + 1, i + 1, "Three of a kind");
//            }
//        }
//        else if (counts[i] == 2) {
//            for (j = 0; j < MAX_DIE_NUM; j++) {
//                if (counts[j] == 2 && i != j && score < 2) {
//                    set_score(&score, which_cards, what_combination, 2, i + 1, j + 1, "Two pair");
//                }
//            }
//            if (score < 1) {
//                set_score(&score, which_cards, what_combination, 1, i + 1, i + 1, "One pair");
//            }
//        }
//        else {
//            if (counts[0] == 1 && counts[1] == 1 && counts[2] == 1 && counts[3] == 1 && counts[4] == 1 && score < 4) {
//                set_score(&score, which_cards, what_combination, 4, 5, 5, "Small straight");
//            }
//            else if (counts[1] == 1 && counts[2] == 1 && counts[3] == 1 && counts[4] == 1 && counts[5] == 1 && score < 5) {
//                set_score(&score, which_cards, what_combination, 5, 6, 6, "Big straight");
//            }
//            else if (score == 0) {
//                set_score(&score, which_cards, what_combination, 0, 6, 6, "Nothing");
//            }
//        }
//    }
//    printf("%s\n", what_combination);
//    if (which_cards[0] != which_cards[1]) printf("%d %d", which_cards[0], which_cards[1]);
//    else printf("%d", which_cards[0]);
//    return score;
//}
//
//int *init_player() {
//    int i;
//    int *hand = (int *) calloc(NUMBER_OF_DICE, sizeof(int));
//    int indices[NUMBER_OF_DICE];
//    for (i = 0; i < NUMBER_OF_DICE; i++) {
//        indices[i] = i;
//    }
//    reroll_hand(hand, indices, NUMBER_OF_DICE);
//    return hand;
//}

int main() {
//    int i;
//    srand(time(NULL));
//    int *hand = init_player();
//    print_array(hand, NUMBER_OF_DICE);
//    int how_many = 0;
//    printf("\nEnter how many dice would you like to reroll:");
//    scanf("%d", &how_many);
//    int reroll_indices[how_many];
//    for (i = 0; i < how_many; i++) {
//        printf("Please enter %d. index:", i+1);
//        scanf("%d", &reroll_indices[i]);
//    }
//    printf("\n");
//    reroll_hand(hand, reroll_indices, how_many);
//    print_array(hand, NUMBER_OF_DICE);
//    evaluate_hand(hand);
//    free(hand);

    std::cout << "Hello, World!" << std::endl;

    std::cout << P_S_IN_GAME_CAN_PLAY << std::endl;

    std::cout << StateMachine::is_transition_possible(G_S_FINISHED, G_E_END_ROUND) << std::endl;

    return EXIT_SUCCESS;
}
