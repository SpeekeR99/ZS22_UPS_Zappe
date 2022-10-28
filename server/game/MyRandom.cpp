#include "MyRandom.h"

MyRandom::MyRandom() {
    gen.seed(time(nullptr));
    dis = std::uniform_int_distribution<>(MIN_DIE_NUM, MAX_DIE_NUM);
}

int MyRandom::roll_a_die() {
    return dis(gen);
}
