#include "random.h"

void init_random() {
    srand(time(NULL));
}

byte roll_a_die() {
    return (rand() % (MAX_DIE_NUM - MIN_DIE_NUM + 1)) + MIN_DIE_NUM;
}