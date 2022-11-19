#pragma once

#include <random>
#include <ctime>

/** Maximum value of a die. */
constexpr unsigned char MAX_DIE_NUM = 6;
/** Minimum value of a die. */
constexpr unsigned char MIN_DIE_NUM = 1;

/**
 * Class for generating random numbers of a die
 */
class MyRandom {
private:
    /** Random number generator. */
    std::default_random_engine gen;
    /** Distribution of random numbers. */
    std::uniform_int_distribution<> dis;

public:
    /**
     * Constructor for MyRandom
     */
    MyRandom();

    /**
     * Destructor for MyRandom (default)
     */
    ~MyRandom() = default;

    /**
     * Generates a random number between MIN_DIE_NUM and MAX_DIE_NUM
     * @return random number between MIN_DIE_NUM and MAX_DIE_NUM
     */
    int roll_a_die();
};
