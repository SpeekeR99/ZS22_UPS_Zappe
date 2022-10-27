#pragma once

#include <random>

constexpr unsigned char MAX_DIE_NUM = 6;
constexpr unsigned char MIN_DIE_NUM = 1;

class RandomCPP {
private:
    std::default_random_engine gen;
    std::uniform_int_distribution<> dis;

public:
    RandomCPP();
    ~RandomCPP() = default;

    int roll_a_die();
};
