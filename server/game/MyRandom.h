#pragma once

#include <random>
#include <ctime>

constexpr unsigned char MAX_DIE_NUM = 6;
constexpr unsigned char MIN_DIE_NUM = 1;

class MyRandom {
private:
    std::default_random_engine gen;
    std::uniform_int_distribution<> dis;

public:
    MyRandom();
    ~MyRandom() = default;

    int roll_a_die();
};
