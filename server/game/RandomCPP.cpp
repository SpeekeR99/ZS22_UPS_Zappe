#include "RandomCPP.h"

RandomCPP::RandomCPP() {
    gen.seed(time(nullptr));
    dis = std::uniform_int_distribution<>(MIN_DIE_NUM, MAX_DIE_NUM);
}

int RandomCPP::roll_a_die() {
    return dis(gen);
}
