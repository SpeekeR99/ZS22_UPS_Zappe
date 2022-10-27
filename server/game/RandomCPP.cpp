#include "RandomCPP.h"

RandomCPP::RandomCPP() {
    std::random_device rd;
    gen = std::default_random_engine(rd());
    dis = std::uniform_int_distribution<>(MIN_DIE_NUM, MAX_DIE_NUM);
}

int RandomCPP::roll_a_die() {
    return dis(gen);
}
