#include <random>
#include <ctime>

class RandomGenerator {
    std :: random_device rd;
    std :: mt19937 gen;
    std :: uniform_int_distribution <> distribution;
    std :: uniform_real_distribution <> distribution2;

public:
    RandomGenerator(int a, int b) {
        gen = std :: mt19937(std :: time(nullptr));
        this -> distribution = std :: uniform_int_distribution <> (a, b);
        this -> distribution2 = std :: uniform_real_distribution <> (a, b);
    }

    /* Returns a random integer in [a, b] */
    int getRandomInteger() {
        return this -> distribution(gen);
    }

    /* Returns a random double in [a, b] */
    double getRandomDouble() {
        return this -> distribution2(gen);
    }
};
