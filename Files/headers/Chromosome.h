#include <vector>
#include <iostream>
#include <numeric>
#include <string>

struct Chromosome {
    std :: vector <bool> genes;

    Chromosome(int length) : genes(length) {}

    Chromosome(const Chromosome *copy) {
        this -> genes = copy -> genes;
    }

    /* Transforms a binary vector into a number in base 10 */
    double getBase10() {
        return std :: accumulate(genes.begin(), genes.end(), 0ull, [] (auto acc, auto bit) {
            return (acc << 1) | bit;
        });
    }

    /* Stringify a binary vector */
    std :: string toString() {
        std :: string ans;

        for (auto b : genes)
            ans += b ? "1" : "0";

       return ans;
    }
};
